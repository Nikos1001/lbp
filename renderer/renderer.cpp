
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

void main() {
    gl_Position = uProjView * uModel * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    pUv = aUv / 5; 
    mat3 normalTransform = mat3(transpose(inverse(uModel)));
    pNorm = normalTransform * aNorm;
    pTang = normalTransform * aTang;
    pBitang = cross(aNorm, aTang);
}

);

static const char* meshShaderFragSrc = "#version 330 core\n" str(

in vec2 pUv;
in vec3 pNorm;
in vec3 pTang;
in vec3 pBitang;
out vec4 oColor;

uniform sampler2D uCol;
uniform sampler2D uNorm;

vec3 lerp(vec3 a, vec3 b, float x) {
    return (1 - x) * a + x * b;
}

void main() {
    vec3 mapNorm = texture(uNorm, pUv).xyz * 2 - 1;
    mapNorm = mapNorm.y * pTang + mapNorm.x * pBitang + mapNorm.z * pNorm;
    mapNorm = normalize(mapNorm);
    vec3 rawNorm = pNorm; 
    vec3 norm = lerp(rawNorm, mapNorm, 1);
    norm = normalize(norm);

    vec3 lightDir = normalize(vec3(-1.0f, 5.0f, 7.0f));
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0f, 1.0f, 1.5f);
    vec4 light = vec4(diffuse, 1.0f) + vec4(0.7f, 0.7f, 0.7f, 1.0f);

    oColor = light * texture(uCol, pUv); 
    //oColor = vec4(norm / 2 + 0.5, 1.0f);
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

void Renderer::endFrame() {
    glfwSwapBuffers(window);
    glfwPollEvents();
}
    
