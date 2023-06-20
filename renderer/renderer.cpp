
#include "renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include "opengl.h"
#include "stb_image.h"
#include "../material.h"

static const char* meshShaderVertSrc = "#version 330 core\n" str(

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUv;
layout (location = 2) in vec3 aNorm;
layout (location = 3) in vec3 aTang;
layout (location = 4) in float aTexId;

uniform mat4 uProjView;
uniform mat4 uModel;

out vec2 pUv;
out vec3 pNorm;
out vec3 pTang;
out vec3 pBitang;
out vec3 pPos;
out float pTexId;

void main() {
    gl_Position = uProjView * uModel * vec4(aPos, 1.0f);
    pPos = (uModel * vec4(aPos, 1.0f)).xyz; 
    pUv = aUv; 
    mat3 normalTransform = mat3(transpose(inverse(uModel)));
    pNorm = normalTransform * aNorm;
    pTang = normalTransform * aTang;
    pBitang = cross(aNorm, aTang);
    pTexId = aTexId;
}

);

static const char* meshShaderFragSrc = "#version 330 core\n" "#define PI 3.14159\n" "#define NUM_LIGHTS 4\n" str(

in vec2 pUv;
in vec3 pNorm;
in vec3 pTang;
in vec3 pBitang;
in vec3 pPos;
in float pTexId;

layout (location = 0) out vec4 oColor;
layout (location = 1) out vec3 oNorm;
layout (location = 2) out vec3 oPos;
layout (location = 3) out vec3 oErm;

uniform vec3 uCamPos;

uniform sampler2D uCol[3];
uniform sampler2D uNorm[3];
uniform sampler2D uArm[3];

uniform vec3 uAmbient;

uniform float uNormStrength;
uniform float uEmission;

vec4 sampleCol(vec2 uv, int id) {
    if(id == 0)
        return texture(uCol[0], uv);
    if(id == 1)
        return texture(uCol[1], uv);
    if(id == 2)
        return texture(uCol[2], uv);
}

vec4 sampleNorm(vec2 uv, int id) {
    if(id == 0)
        return texture(uNorm[0], uv);
    if(id == 1)
        return texture(uNorm[1], uv);
    if(id == 2)
        return texture(uNorm[2], uv);
}

vec4 sampleArm(vec2 uv, int id) {
    if(id == 0)
        return texture(uArm[0], uv);
    if(id == 1)
        return texture(uArm[1], uv);
    if(id == 2)
        return texture(uArm[2], uv);
}

void main() {
    vec3 meshNorm = normalize(pNorm);
    vec3 tang = normalize(pTang);
    vec3 bitang = normalize(pBitang);
    vec2 uv = pUv;

    int texId = int(pTexId);

    vec3 mapNorm = normalize(sampleNorm(uv, texId).xyz * 2 - 1);
    mapNorm = normalize(-mapNorm.y * tang + mapNorm.x * bitang + mapNorm.z * meshNorm);
    vec3 norm = normalize(mix(meshNorm, mapNorm, uNormStrength));

    vec3 view = normalize(uCamPos - pPos);
    
    vec3 matCol = sampleCol(uv, texId).xyz;
    matCol.x = pow(matCol.x, 2.2f);
    matCol.y = pow(matCol.y, 2.2f);
    matCol.z = pow(matCol.z, 2.2f);
    vec3 arm = sampleArm(uv, texId).xyz;

    vec3 erm = arm;
    erm.x = uEmission;

    oColor = vec4(matCol, 1.0f); 
    oNorm = norm;
    oPos = pPos;
    oErm = erm;

} 

);

const char* screenShaderVertSrc = "#version 330 core\n" str(

layout (location = 0) in vec3 aPos;

out vec2 pUv;

void main() {
    gl_Position = vec4(2 * aPos, 1.0f);
    pUv = aPos.xy + 0.5f;
}

); 

const char* screenShaderFragSrc = "#version 330 core\n#define PI 3.14159\n" str(

in vec2 pUv;

uniform sampler2D uCol;
uniform sampler2D uNorm;
uniform sampler2D uPos;
uniform sampler2D uErm;

struct PointLight {
    vec3 pos;    
    vec3 col;
};

uniform PointLight uPointLights[4];

uniform vec3 uDirectionalDir; 
uniform vec3 uDirectionalCol;

uniform vec3 uCamPos;

out vec4 oColor;

float sq(float a) {
    return a * a;
}

vec3 lerp(vec3 a, vec3 b, float x) {
    return (1 - x) * a + x * b;
}

float dot0(vec3 a, vec3 b) {
    return max(dot(a, b), 0.0f);
}

vec3 fresnel(vec3 v, vec3 h, vec3 f0) {
    return f0 + (1.0f - f0) * pow(1.0f - dot0(v, h), 5.0f);
}

float distribution(float alpha, vec3 n, vec3 h) {
    float num = sq(alpha);
    float denom = PI * sq(sq(dot0(n, h)) * (sq(alpha) - 1) + 1);
    denom = max(denom, 0.000001f);
    return num / denom;
}

float g(vec3 n, vec3 x, float k) {
    float num = dot0(n, x);
    float denom = dot0(n, x) * (1 - k) + k;
    denom = max(denom, 0.000000001f);
    return num / denom;
}

float shadowing(vec3 l, vec3 v, vec3 n, float alpha) {
    float k = alpha / 2.0f;
    return g(n, l, k) * g(n, v, k);
}

vec3 specular(float alpha, vec3 n, vec3 h, vec3 l, vec3 v, vec3 f) {
    vec3 num = distribution(alpha, n, h) * shadowing(l, v, n, alpha) * f;
    float denom = 4 * dot0(v, n) * dot0(l, n);
    denom = max(denom, 0.00000001f);
    return num / denom;
}

vec3 calcLighting(vec3 l, vec3 n, vec3 v, vec3 matCol, vec3 incomingLight, vec3 erm) {
    vec3 h = normalize(l + v);

    float roughness = erm.y; 
    float metallic = erm.z; 
    
    float alpha = sq(roughness);

    vec3 f0 = mix(vec3(0.04f), matCol, metallic);
    vec3 f = fresnel(v, h, f0);
    vec3 kd = 1 - f; 

    vec3 lambert = matCol / PI;

    vec3 specular = specular(alpha, n, h, l, v, f);

    vec3 brdf = kd * lambert + specular; 
    vec3 light = brdf * incomingLight * dot0(l, n);

    return light;
}


void main() {

    vec3 matCol = texture(uCol, pUv).xyz; 
    vec3 norm = texture(uNorm, pUv).xyz; 
    vec3 pos = texture(uPos, pUv).xyz;
    vec3 erm = texture(uErm, pUv).xyz;

    vec3 view = normalize(uCamPos - pos);
    
    vec3 light = erm.x * matCol; 
    light += calcLighting(-uDirectionalDir, norm, view, matCol, uDirectionalCol, erm);
    for(int i = 0; i < 1; i++) {
        vec3 lightPos = uPointLights[i].pos;
        vec3 lightDir = normalize(lightPos - pos);
        vec3 halfView = normalize(lightDir + view);
        float dist = length(lightPos - pos);
        float power = 1 / (1 + dist + 2 * dist * dist);
        light += calcLighting(lightDir, norm, view, matCol, uPointLights[i].col * power, erm);
    }

    // High-tech HDR mapping
    light = light / (light + vec3(1.0f));

    oColor = vec4(light, 1.0f); 
}

);

void Renderer::init() {

    glewExperimental = true;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        printf("GLFW ded :(\n");
        exit(-1);
    }
    glfwMakeContextCurrent(window);

    if(glewInit() != GLEW_OK) {
        printf("GLEW ded :(\n");
        exit(-1);
    }

    meshShader.init(meshShaderVertSrc, meshShaderFragSrc);
    meshShader.use();
    meshShader.setIntUniform("uCol[0]", 0);
    meshShader.setIntUniform("uNorm[0]", 1);
    meshShader.setIntUniform("uArm[0]", 2); 
    meshShader.setIntUniform("uCol[1]", 3);
    meshShader.setIntUniform("uNorm[1]", 4);
    meshShader.setIntUniform("uArm[1]", 5);
    meshShader.setIntUniform("uCol[2]", 6);
    meshShader.setIntUniform("uNorm[2]", 7);
    meshShader.setIntUniform("uArm[2]", 8);

    screenShader.init(screenShaderVertSrc, screenShaderFragSrc);
    screenShader.use();
    screenShader.setIntUniform("uCol", 0);
    screenShader.setIntUniform("uNorm", 1);
    screenShader.setIntUniform("uPos", 2);
    screenShader.setIntUniform("uErm", 3);

    quad.init();

    MeshVert verts[] = {
        (MeshVert){glm::vec3(0.5f,  0.5f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
        (MeshVert){glm::vec3(0.5f, -0.5f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
        (MeshVert){glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
        (MeshVert){glm::vec3(-0.5f,  0.5f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f)}
    };

    unsigned int indices[] = { 
        0, 1, 3,
        1, 2, 3
    };

    quad.upload(4, verts, 2, indices);

    glEnable(GL_DEPTH_TEST);
    stbi_set_flip_vertically_on_load(true);

    gbuff.init(800, 600);

}

void Renderer::beginFrame() {

    int viewW, viewH;
    glfwGetWindowSize(window, &viewW, &viewH);
    float xScale, yScale;
    glfwGetWindowContentScale(window, &xScale, &yScale);
    viewW *= xScale;
    viewH *= yScale;
    gbuff.resize(viewW, viewH);

    gbuff.renderTo();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, -camPos);

    int w, h;
    glfwGetWindowSize(window, &w, &h);
    glm::mat4 proj = glm::perspective(glm::radians(90.0f), (float)w / (float)h, 0.1f, 1000.0f);

    projView = proj * view;

    meshShader.use();
    meshShader.setMat4Uniform("uProjView", projView);

}

void Renderer::beginLighting() {
    screenShader.use();
    currLight = 0;
    for(int i = 0; i < 16; i++) {
        char buf[128];
        sprintf(buf, "uPointLights[%d].col", i);
        screenShader.setVec3Uniform(buf, glm::vec3(0.0f, 0.0f, 0.0f));
    }
}

void Renderer::endLighting() {
    meshShader.use();
}

void Renderer::endFrame() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // glBindFramebuffer(GL_READ_FRAMEBUFFER, gbuff.fbo);
    // glBlitFramebuffer(// makes OpenGL copying the framebuffer data
    // 0, 0, 800, 600,
    // 0, 0, 800, 600,
    // GL_COLOR_BUFFER_BIT,// you only care about the color data, not the depth data …
    // GL_NEAREST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    screenShader.use();
    screenShader.setVec3Uniform("uCamPos", camPos);
    gbuff.col.use(0);
    gbuff.norm.use(1);
    gbuff.pos.use(2);
    gbuff.erm.use(3);
    quad.render();

    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Renderer::renderMesh(Mesh& mesh, int nTexs, Texture* col, Texture* norm, Texture* arm, glm::mat4 model, float uvScale, float normStrength, float emission) {
    meshShader.use();
    for(int i = 0; i < nTexs; i++) {
        col[i].use(3 * i);
        norm[i].use(3 * i + 1);
        arm[i].use(3 * i + 2);
    }
    meshShader.setMat4Uniform("uModel", model);
    meshShader.setFloatUniform("uNormStrength", normStrength);
    meshShader.setFloatUniform("uEmission", emission);
    mesh.render();
}

void Renderer::renderMesh(Mesh& mesh, Texture& col, Texture& norm, Texture& arm, glm::mat4 model, float uvScale, float normStrength, float emission) {
    meshShader.use();
    col.use(0);
    norm.use(1);
    arm.use(2);
    meshShader.setMat4Uniform("uModel", model);
    meshShader.setFloatUniform("uNormStrength", normStrength);
    meshShader.setFloatUniform("uEmission", emission);
    mesh.render();
}
    
void Renderer::setAmbient(glm::vec3 ambient) {
    meshShader.setVec3Uniform("uAmbient", ambient);
}

void Renderer::setDirectional(glm::vec3 dir, glm::vec3 col) {
    screenShader.setVec3Uniform("uDirectionalDir", dir);
    screenShader.setVec3Uniform("uDirectionalCol", col);
}

void Renderer::addPointLight(glm::vec3 pos, glm::vec3 col) {
    char buf[128];
    sprintf(buf, "uPointLights[%d].pos", currLight);
    screenShader.setVec3Uniform(buf, pos);
    sprintf(buf, "uPointLights[%d].col", currLight);
    screenShader.setVec3Uniform(buf, col);
    currLight++;
}
