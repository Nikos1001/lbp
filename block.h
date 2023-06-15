
#ifndef BLOCK_H
#define BLOCK_H

#include "renderer/mesh.h"
#include "polygon/polygon.h"
#include "box2d.h"

struct BlockPiece {
    bool isModelMat;
    int frontLayer;
    int backLayer;
    int material;

    // poly
    Polygon<true> poly;
    Mesh mesh;

    // model
    glm::vec2 pos;
};

class Block {

public:

    void init(bool dynamic);
    void free();
    void render();

    void updateMesh();
    int addPiece(bool modelMat, int frontLayer, int backLayer, int material); 

    Arr<BlockPiece, true> pieces;

    b2Body* body;

};

#endif
