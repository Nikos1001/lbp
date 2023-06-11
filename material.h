
#ifndef MATERIAL_H
#define MATERIAL_H

#include "renderer/texture.h"

enum MaterialMeshGen {
    FLAT,
    SQUARE_BEVEL
};

struct Material {
    const char* name;
    float density;
    MaterialMeshGen meshGen; 
    float bevelWidth;
    float faceInset;
    Texture col;
    Texture norm;
    Texture arm;
};

extern Material materials[];
extern int nMats;

void loadMaterials();

#endif
