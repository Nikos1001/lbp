
#include "mesh.h"
#include <GL/glew.h>

void Mesh::init() {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    glGenBuffers(1, &vert);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &ebo);

}

void Mesh::free() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vert);
    glDeleteBuffers(1, &ebo);
}

void Mesh::upload(int nVerts, float* verts, int nTris, unsigned int* idxs) {
    this->nTris = nTris;
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vert);
    glBufferData(GL_ARRAY_BUFFER, nVerts * 3 * sizeof(float), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * nTris * 3, idxs, GL_STATIC_DRAW);
}

void Mesh::render() {
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glDrawElements(GL_TRIANGLES, 3 * nTris, GL_UNSIGNED_INT, 0);
}
