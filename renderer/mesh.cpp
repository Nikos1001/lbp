
#include "mesh.h"
#include <GL/glew.h>

static void configAttribs() {
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
}

void Mesh::init() {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    glGenBuffers(1, &vert);
    configAttribs();

    glGenBuffers(1, &ebo);

}

void Mesh::free() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vert);
    glDeleteBuffers(1, &ebo);
}

void Mesh::upload(int nVerts, MeshVert* verts, int nTris, unsigned int* idxs) {
    this->nTris = nTris;
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vert);
    glBufferData(GL_ARRAY_BUFFER, nVerts * sizeof(MeshVert), verts, GL_STATIC_DRAW);
    configAttribs();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * nTris * 3, idxs, GL_STATIC_DRAW);
}

void Mesh::render() {
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glDrawElements(GL_TRIANGLES, 3 * nTris, GL_UNSIGNED_INT, 0);
}
