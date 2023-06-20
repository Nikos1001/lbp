
#ifndef TEXTURE_H
#define TEXTURE_H

enum TextureType {
    COLOR,
    DEPTH_STENCIL,
};

class Texture {

public:

    void init();
    void free();
    void create(int w, int h, TextureType type);
    void loadFromFile(const char* path);
    void use(int slot);

    unsigned int tex;

};

#endif
