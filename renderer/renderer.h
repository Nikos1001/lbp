
#ifndef RENDERER_H
#define RENDERER_H

#include "opengl.h"
#include "../util.h"
#include "shader.h"
#include "mesh.h"

class Renderer {

public:

    void init();
    void beginFrame();
    void endFrame();

    GLFWwindow* window;

    glm::vec3 camPos;
    glm::mat4 projView;

    Shader meshShader;
    Mesh quad;

};

#endif
