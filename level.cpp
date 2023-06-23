
#include "level.h"
#include "block.h"
#include "objList.h"
#include "polygon/polygon.h"

LevelPieceVertex makePointVertex(glm::vec2 point) {
    LevelPieceVertex res;
    res.point = point;
    return res;
}

void LevelPiece::init() {
    vertices.init();
    frontLayer = backLayer = 0;
    material = 0;
    tombstone = false;
}

void LevelPiece::free() {
    vertices.free();
    tombstone = true;
}

void LevelBlock::init() {
    pieceIdxs.init();
    block = NULL;
    tombstone = false;
}

void LevelBlock::free() {
    pieceIdxs.free();
    block = NULL;
    tombstone = true;
}

static void computePiecePolygon(LevelPiece* piece, Polygon<true>* polygon) {
    int start = polygon->addChain(piece->vertices[0].point); 
    int v = start;
    for(int i = 1; i < piece->vertices.cnt(); i++) {
        v = polygon->addPoint(piece->vertices[i].point, v);
    }
    polygon->closeChain(start, v);
}

void LevelBlock::update(Level* level) {
    if(block != NULL)
        ObjList<Block>::kill(block);
    block = ObjList<Block>::spawn();
    block->init(false);
    for(int i = 0; i < pieceIdxs.cnt(); i++) {
        LevelPiece* piece = &level->pieces[pieceIdxs[i]];
        int pieceIdx = block->addPiece(false, piece->frontLayer, piece->backLayer, 0);
        block->pieces[pieceIdx].material = piece->material;
        computePiecePolygon(piece, &block->pieces[pieceIdx].poly);
    }
    block->updateMesh();
}

void Level::init() {
    skyCol = glm::vec3(0.6f, 0.7, 1.0f);
    dirCol = glm::vec3(4.0f);

    pieces.init();
    activePieces = 0;
    blocks.init();
    activeBlocks = 0;
}

void Level::free() {
    for(int i = 0; i < pieces.cnt(); i++)
        pieces[i].free();
    pieces.free();
    for(int i = 0; i < blocks.cnt(); i++)
        blocks[i].free();
    blocks.free();
}

int Level::addPiece(int block) {
    int idx = -1;
    for(int i = 0; i < pieces.cnt(); i++) {
        if(pieces[i].tombstone) {
            idx = i;
            break;
        }
    }
    if(idx == -1) {
        LevelPiece piece;
        pieces.add(piece);
        idx = pieces.cnt() - 1;
    }
    pieces[idx].init();
    pieces[idx].block = block;
    pieces[idx].idxInBlock = blocks[block].pieceIdxs.cnt();
    blocks[block].pieceIdxs.add(idx);
    activePieces++;
    return idx; 
}

void Level::deleteBlock(int blockIdx) {
    LevelBlock* block = &blocks[blockIdx];
    if(block->block != NULL) {
        ObjList<Block>::kill(block->block);
        block->block = NULL;
    }
    block->free();
    activeBlocks--;
}

void Level::deletePiece(int pieceIdx) {
    LevelPiece* piece = &pieces[pieceIdx];
    LevelBlock* block = &blocks[piece->block];
    if(block->pieceIdxs.cnt() == 1) {
        deleteBlock(piece->block);
    } else {
        for(int i = 0; i < block->pieceIdxs.cnt(); i++) {
            if(block->pieceIdxs[i] == pieceIdx) {
                block->pieceIdxs[i] = block->pieceIdxs[block->pieceIdxs.cnt() - 1];
                block->pieceIdxs.pop();
                pieces[block->pieceIdxs[i]].idxInBlock = piece->idxInBlock;
            } 
        }
    }
    piece->free();
    activePieces--;
}

int Level::addBlock() {
    int idx = -1;
    for(int i = 0; i < blocks.cnt(); i++) {
        if(blocks[i].tombstone) {
            idx = i;
            break;
        }
    }
    if(idx == -1) {
        LevelBlock block;
        blocks.add(block);
        idx = blocks.cnt() - 1;
    }
    blocks[idx].init();
    activeBlocks++;
    return idx;
}
