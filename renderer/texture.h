
#ifndef TEXTURE_H
#define TEXTURE_H

class Texture {

public:

    void init();
    void free();
    void loadFromFile(const char* path);
    void use(int slot);

private:
    
    unsigned int tex;

};

#endif
