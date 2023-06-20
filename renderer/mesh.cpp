
#include "mesh.h"
#include <GL/glew.h>

static void configAttribs() {
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(11 * sizeof(float)));
    glEnableVertexAttribArray(4);
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

#include "../lib/assimp/include/assimp/Importer.hpp"
#include "../lib/assimp/include/assimp/scene.h"
#include "../lib/assimp/include/assimp/postprocess.h"
void Mesh::loadFrom(const char* path) {

    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_GenUVCoords); 

    aiMesh* mesh = scene->mMeshes[0];
    int nVerts = mesh->mNumVertices;
    MeshVert verts[nVerts];
    for(int i = 0; i < nVerts; i++) {
        verts[i].pos.x = mesh->mVertices[i].x;
        verts[i].pos.y = mesh->mVertices[i].y;
        verts[i].pos.z = mesh->mVertices[i].z;
        verts[i].uv.x = mesh->mTextureCoords[0][i].x;
        verts[i].uv.y = mesh->mTextureCoords[0][i].y;
        verts[i].norm.x = mesh->mNormals[i].x;
        verts[i].norm.y = mesh->mNormals[i].y;
        verts[i].norm.z = mesh->mNormals[i].z;
        verts[i].tang.x = mesh->mTangents[i].x;
        verts[i].tang.y = mesh->mTangents[i].y;
        verts[i].tang.z = mesh->mTangents[i].z;
        verts[i].texId = 0.0f;
    }

    int numTris = mesh->mNumFaces;
    unsigned int indicies[numTris * 3];
    for(int i = 0; i < numTris; i++) {
        indicies[3 * i + 0] = mesh->mFaces[i].mIndices[0];
        indicies[3 * i + 1] = mesh->mFaces[i].mIndices[1];
        indicies[3 * i + 2] = mesh->mFaces[i].mIndices[2];
    }

    init();
    upload(nVerts, verts, numTris, indicies);
}