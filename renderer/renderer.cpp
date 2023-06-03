
#include "renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

static const char* meshShaderVertSrc = "#version 330 core\n" str(

layout (location = 0) in vec3 aPos;

uniform mat4 uTrans;

void main() {
    gl_Position = uTrans * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}

);

static const char* meshShaderFragSrc = "#version 330 core\n" str(

out vec4 oColor;

void main() {
    oColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
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

    float vertices[] = {
        0.5f,  0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };

    unsigned int indices[] = { 
        0, 1, 3,
        1, 2, 3
    };

    quad.upload(4, vertices, 2, indices);

}

void Renderer::beginFrame() {

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, -camPos);

    int w, h;
    glfwGetWindowSize(window, &w, &h);
    glm::mat4 proj = glm::perspective(glm::radians(90.0f), (float)w / (float)h, 0.1f, 1000.0f);

    projView = proj * view;
}

void Renderer::endFrame() {
    glfwSwapBuffers(window);
    glfwPollEvents();
}
    
