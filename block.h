
#ifndef BLOCK_H
#define BLOCK_H

#include "renderer/mesh.h"
#include "polygon/polygon.h"
#include "box2d.h"

class Block {

public:

    void init(glm::vec2 pos);
    void free();
    void render();

    void updateMesh();

    void setDynamic(bool dynamic);

    Polygon<true> poly;

private:

    Mesh mesh;
    b2Body* body;

};

#endif
