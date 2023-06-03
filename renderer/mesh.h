
#ifndef MESH_H
#define MESH_H

class Mesh {

public:
    void init();
    void free();
    void upload(int nVerts, float* verts, int tris, unsigned int* idxs);
    void render();

private:
    unsigned int vao;
    unsigned int vert;
    unsigned int ebo;
    unsigned int nTris;

};

#endif
