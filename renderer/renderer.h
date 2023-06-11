
#ifndef RENDERER_H
#define RENDERER_H

#include "opengl.h"
#include "../util.h"
#include "shader.h"
#include "texture.h"
#include "mesh.h"

class Renderer {

public:

    void init();
    void beginFrame();
    void beginLighting();
    void endLighting();
    void endFrame();

    void renderMesh(Mesh& mesh, Texture& col, Texture& norm, glm::mat4 model);

    void setAmbient(glm::vec3 ambient);
    void setDirectional(glm::vec3 dir, glm::vec3 col);
    void addPointLight(glm::vec3 pos, glm::vec3 col);

    GLFWwindow* window;

    glm::vec3 camPos;
    glm::mat4 projView;

    Shader meshShader;
    Mesh quad;

private:

    int currLight;

};

#endif
