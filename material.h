
#ifndef MATERIAL_H
#define MATERIAL_H

#include "renderer/texture.h"
#include "renderer/mesh.h"
#include "polygon/polygon.h"

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
};

extern Material materials[];
extern int nMats;

void loadMaterials();

struct ModelMaterial {
    const char* name;
    float density;
    int nColliderVerts;
    float* colliderVerts;

    Mesh mesh;
    Texture col;
    Texture norm;
    Polygon<true> polygon;
};

extern ModelMaterial modelMaterials[];
extern int nModelMats;

#endif
