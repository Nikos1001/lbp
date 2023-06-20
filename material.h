
#ifndef MATERIAL_H
#define MATERIAL_H

#include "renderer/texture.h"
#include "renderer/mesh.h"
#include "polygon/polygon.h"
#include "common/mem.h"

enum MaterialMeshGen {
    FLAT,
    SQUARE_BEVEL
};

struct Material {
    char name[64];
    float density;

    MaterialMeshGen meshGen; 
    float bevelWidth;
    float faceInset;

    float uvScale;
    float normStrength;

    Texture col[3];
    Texture norm[3];
    Texture arm[3];
};

extern Arr<Material, true> materials; 

void loadMaterials();

struct ModelMaterial {
    const char* name;
    float density;
    int nColliderVerts;
    float* colliderVerts;
    float emission;

    Mesh mesh;
    Texture col;
    Texture norm;
    Texture arm;
    Polygon<true> polygon;
};

extern ModelMaterial modelMaterials[];
extern int nModelMats;

#endif
