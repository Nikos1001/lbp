
#ifndef SHADER_H
#define SHADER_H

#include <glm/glm.hpp>

class Shader {

public:
    void init(const char* vert, const char* frag);
    void use();

    void setIntUniform(const char* name, int val);
    void setMat4Uniform(const char* name, glm::mat4& val);

private:
    unsigned int program;
    unsigned int getUniformLoc(const char* name);

};

#endif
