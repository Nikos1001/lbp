
#ifndef LEVEL_H
#define LEVEL_H

#include "common/util.h"
#include "common/mem.h"
#include "block.h"

class Level;

class LevelPieceVertex {
public:
    glm::vec2 point; 
};

LevelPieceVertex makePointVertex(glm::vec2 point);

class LevelPiece {
public:
    void init();
    void free();

    Arr<LevelPieceVertex, true> vertices;
    int frontLayer;
    int backLayer;

    int material;

    int block;
    int idxInBlock;

    // To maintain index coherency, we don't remove elements from the level's list of things
    // we just mark it as deleted.
    bool tombstone;
};

class LevelBlock {
public:
    void init();
    void free();
    void update(Level* level);

    Arr<int, true> pieceIdxs;

    Block* block;

    bool tombstone;
};

class Level {

public:

    void init();
    void free();

    glm::vec3 skyCol;
    glm::vec3 dirCol;

    int activePieces;
    Arr<LevelPiece, true> pieces;
    int activeBlocks;
    Arr<LevelBlock, true> blocks;

    int addBlock();
    int addPiece(int block);
    void deletePiece(int piece);

private:
    void deleteBlock(int block);

};

#endif
