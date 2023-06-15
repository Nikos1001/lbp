
#include "renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include "opengl.h"
#include "stb_image.h"

static const char* meshShaderVertSrc = "#version 330 core\n" str(

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUv;
layout (location = 2) in vec3 aNorm;
layout (location = 3) in vec3 aTang;

uniform mat4 uProjView;
uniform mat4 uModel;

out vec2 pUv;
out vec3 pNorm;
out vec3 pTang;
out vec3 pBitang;
out vec3 pPos;

void main() {
    gl_Position = uProjView * uModel * vec4(aPos, 1.0f);
    pPos = (uModel * vec4(aPos, 1.0f)).xyz; 
    pUv = aUv; 
    mat3 normalTransform = mat3(transpose(inverse(uModel)));
    pNorm = normalTransform * aNorm;
    pTang = normalTransform * aTang;
    pBitang = cross(aNorm, aTang);
}

);

static const char* meshShaderFragSrc = "#version 330 core\n" "#define NUM_LIGHTS 16\n" str(

in vec2 pUv;
in vec3 pNorm;
in vec3 pTang;
in vec3 pBitang;
in vec3 pPos;
out vec4 oColor;

uniform sampler2D uCol;
uniform sampler2D uNorm;

uniform vec3 uAmbient;

uniform vec3 uDirectionalDir; 
uniform vec3 uDirectionalCol;

struct PointLight {
    vec3 pos;    
    vec3 col;
};

uniform PointLight uPointLights[NUM_LIGHTS];

vec3 lerp(vec3 a, vec3 b, float x) {
    return (1 - x) * a + x * b;
}

vec3 calcLighting(vec3 dir, vec3 col, vec3 norm) {
    dir = -normalize(dir);
    float diffuse = max(dot(norm, dir), 0.0f);
    return diffuse * col;
}

void main() {
    vec3 mapNorm = texture(uNorm, pUv).xyz * 2 - 1;
    mapNorm = -mapNorm.y * pTang + mapNorm.x * pBitang + mapNorm.z * pNorm;
    mapNorm = normalize(mapNorm);
    vec3 rawNorm = pNorm; 
    vec3 norm = lerp(rawNorm, mapNorm, 0);
    norm = normalize(norm);
    
    vec3 light = vec3(uAmbient);
    light += calcLighting(uDirectionalDir, uDirectionalCol, norm);
    for(int i = 0; i < NUM_LIGHTS; i++) {
        vec3 dir = pPos - uPointLights[i].pos;
        float dist = length(dir);
        float power = 1 / (1 + dist + 2 * dist * dist);
        light += calcLighting(dir, uPointLights[i].col, norm) * power;
    }

    oColor = vec4(light, 1.0f) * texture(uCol, pUv); 

    // oColor = vec4(norm / 2 + 0.5, 1.0f);
    //oColor = vec4(pTang / 2 + 0.5, 1.0f);
    //oColor = vec4(pUv, 0.0f, 1.0f);
    // oColor = vec4(light * 0.1f, 1.0f);
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
    meshShader.setIntUniform("uCol", 0);
    meshShader.setIntUniform("uNorm", 1);

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

}

void Renderer::beginFrame() {

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
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
    currLight = 0;
    for(int i = 0; i < 16; i++) {
        char buf[128];
        sprintf(buf, "uPointLights[%d].col", i);
        meshShader.setVec3Uniform(buf, glm::vec3(0.0f, 0.0f, 0.0f));
    }
}

void Renderer::endLighting() {

}

void Renderer::endFrame() {
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Renderer::renderMesh(Mesh& mesh, Texture& col, Texture& norm, glm::mat4 model) {
    meshShader.use();
    col.use(0);
    norm.use(1);
    meshShader.setMat4Uniform("uModel", model);
    mesh.render();
}
    
void Renderer::setAmbient(glm::vec3 ambient) {
    meshShader.setVec3Uniform("uAmbient", ambient);
}

void Renderer::setDirectional(glm::vec3 dir, glm::vec3 col) {
    meshShader.setVec3Uniform("uDirectionalDir", dir);
    meshShader.setVec3Uniform("uDirectionalCol", col);
}

void Renderer::addPointLight(glm::vec3 pos, glm::vec3 col) {
    char buf[128];
    sprintf(buf, "uPointLights[%d].pos", currLight);
    meshShader.setVec3Uniform(buf, pos);
    sprintf(buf, "uPointLights[%d].col", currLight);
    meshShader.setVec3Uniform(buf, col);
    currLight++;
}
