
#ifndef MATERIAL_H
#define MATERIAL_H

#include "renderer/texture.h"

struct Material {
    const char* name;
    float density;
    Texture col;
    Texture norm;
    Texture arm;
};

extern Material materials[];
extern int nMats;

void loadMaterials();

#endif
