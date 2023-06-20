
#include "shader.h"
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <glm/gtc/type_ptr.hpp>

static unsigned int compileShader(const char* src, bool frag) {
    unsigned int shader = glCreateShader(frag ? GL_FRAGMENT_SHADER : GL_VERTEX_SHADER); 
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    int success;
    char log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(shader, 512, NULL, log);
        printf("SHADER COMPILATION ERROR:\n%s\n", log);
        exit(-1);
    }

    return shader;
}

void Shader::init(const char* vert, const char* frag) {

    unsigned int vertShader = compileShader(vert, false);
    unsigned int fragShader = compileShader(frag, true);

    program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);  

    int success;
    char log[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(program, 512, NULL, log);
        printf("SHADER LINKING ERROR:\n%s\n", log);
        exit(-1);
    }

}

void Shader::use() {
    glUseProgram(program);
}

unsigned int Shader::getUniformLoc(const char* name) {
    return glGetUniformLocation(program, name);
}

void Shader::setIntUniform(const char* name, int val) {
    glUniform1i(getUniformLoc(name), val);
}

void Shader::setFloatUniform(const char* name, float val) {
    glUniform1f(getUniformLoc(name), val);
}

void Shader::setVec3Uniform(const char* name, glm::vec3 val) {
    glUniform3f(getUniformLoc(name), val.x, val.y, val.z);
}

void Shader::setMat4Uniform(const char* name, glm::mat4& val) {
    glUniformMatrix4fv(getUniformLoc(name), 1, GL_FALSE, glm::value_ptr(val));
}
