
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

    static void init();
    static void beginFrame();
    static void beginLighting();
    static void endLighting();
    static void beginScene();
    static void endScene();
    static void beginUI();
    static void endUI();
    static void endFrame();

    static void renderMesh(Mesh& mesh, int nTexs, Texture* col, Texture* norm, Texture* arm, glm::mat4 model, float uvScale, float normStrength, float emission);
    static void renderMesh(Mesh& mesh, Texture& col, Texture& norm, Texture& arm, glm::mat4 model, float uvScale, float normStrength, float emission);

    static void setAmbient(glm::vec3 ambient);
    static void setDirectional(glm::vec3 dir, glm::vec3 col);
    static void addPointLight(glm::vec3 pos, glm::vec3 col);

    static void drawUILine(glm::vec3 a, glm::vec3 b, float thickness, glm::vec3 color);
    static void drawUICircle(glm::vec3 pt, float radius, glm::vec3 color, glm::vec3 innerColor = glm::vec3(0.0f), float innerRadiusRation = 0.0f);

    static glm::vec3 screenToWorld(glm::vec2 pt, float z);

    static inline GLFWwindow* window;

    static inline glm::vec3 camPos;
    static inline glm::mat4 projView;

    static inline Shader meshShader;
    static inline Shader screenShader;
    static inline Shader uiLineShader;
    static inline Shader uiCircleShader;

    static inline Mesh quad;


private:

    static inline int currLight;
    static inline Framebuffer gbuff;

};

#endif
