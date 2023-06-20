

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "texture.h"
#include "opengl.h"

void Texture::init() {
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::free() {
    glDeleteTextures(1, &tex);
}

void Texture::create(int w, int h, TextureType texType) {
    GLenum internalFormat;
    GLenum format;
    GLenum type;
    switch(texType) {
        case COLOR: {
            internalFormat = GL_RGBA16F;
            format = GL_RGBA;
            type = GL_FLOAT;
            break;
        }
        case DEPTH_STENCIL: {
            internalFormat = GL_DEPTH24_STENCIL8;
            format = GL_DEPTH_STENCIL;
            type = GL_UNSIGNED_INT_24_8;
            break;
        }
    }
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, type, NULL);
}

void Texture::loadFromFile(const char* path) {
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0); 
    if(data == NULL || nrChannels < 3) {
        printf("Texture %s ded :(\n", path);
        exit(-1);
    }

    glBindTexture(GL_TEXTURE_2D, tex);
    GLenum format = nrChannels == 3 ? GL_RGB : GL_RGBA; 
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}

void Texture::use(int slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, tex);
}

