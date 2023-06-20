
#ifndef RENDERER_H
#define RENDERER_H

#include "opengl.h"
#include "../common/util.h"
#include "shader.h"
#include "texture.h"
#include "mesh.h"
#include "framebuffer.h"
class Renderer {

public:

    void init();
    void beginFrame();
    void beginLighting();
    void endLighting();
    void endFrame();

    void renderMesh(Mesh& mesh, int nTexs, Texture* col, Texture* norm, Texture* arm, glm::mat4 model, float uvScale, float normStrength, float emission);
    void renderMesh(Mesh& mesh, Texture& col, Texture& norm, Texture& arm, glm::mat4 model, float uvScale, float normStrength, float emission);

    void setAmbient(glm::vec3 ambient);
    void setDirectional(glm::vec3 dir, glm::vec3 col);
    void addPointLight(glm::vec3 pos, glm::vec3 col);

    GLFWwindow* window;

    glm::vec3 camPos;
    glm::mat4 projView;

    Shader meshShader;
    Shader screenShader;
    Mesh quad;

private:

    int currLight;
    Framebuffer gbuff;

};

#endif
