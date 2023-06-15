
#ifndef MESH_H
#define MESH_H

#include "../common/util.h"

struct MeshVert {
    glm::vec3 pos;
    glm::vec2 uv;
    glm::vec3 norm;
    glm::vec3 tang;
};

class Mesh {

public:
    void init();
    void free();
    void upload(int nVerts, MeshVert* verts, int tris, unsigned int* idxs);
    void render();
    void loadFrom(const char* path);

private:
    unsigned int vao;
    unsigned int vert;
    unsigned int ebo;
    unsigned int nTris;

};

#endif
