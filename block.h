
#ifndef BLOCK_H
#define BLOCK_H

#include "renderer/mesh.h"
#include "polygon/polygon.h"
#include "box2d.h"

struct BlockPiece {
    Polygon<true> poly;
    Mesh mesh;
    int frontLayer;
    int backLayer;
    int material;
};

class Block {

public:

    void init(bool dynamic);
    void free();
    void render();

    void updateMesh();
    int addPiece(int frontLayer, int backLayer, int material); 

    Arr<BlockPiece, true> pieces;

private:

    b2Body* body;

};

#endif
