
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "texture.h"

class Framebuffer {

public:

    void init(int w, int h);
    void free();
    void bind();
    void renderTo();
    void resize(int w, int h);

    Texture col;
    Texture norm;
    Texture pos;
    Texture erm;
    Texture depth;

    unsigned int fbo;

    int w, h;

};

#endif
