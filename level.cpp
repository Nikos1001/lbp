
#include "level.h"
#include "main.h"
#include "util.h"
#include "material.h"

#include <fstream>

void loadModelPiece(json& pieceData, Block* block) {
    std::string modelName = (std::string)pieceData["model"];
    const char* modelNameCStr = modelName.c_str();
    int model = 0;
    for(int i = 0; i < nModelMats; i++)
        if(strcmp(modelMaterials[i].name, modelNameCStr) == 0)
            model = i;
    int pieceIdx = block->addPiece(true, (int)pieceData["layer"], (int)pieceData["layer"], model);
    block->pieces[pieceIdx].pos = glm::vec2((float)pieceData["position"][0], (float)pieceData["position"][1]);
}

void loadPolyPiece(json& pieceData, Block* block) {
    std::string materialName = (std::string)pieceData["material"];
    const char* materialNameCStr = materialName.c_str();
    int material = 0;
    for(int i = 0; i < materials.cnt(); i++)
        if(strcmp(materials[i].name, materialNameCStr) == 0)
            material = i;
    int pieceIdx = block->addPiece(false, (int)pieceData["front"], (int)pieceData["back"], material);
    for(json& chain : pieceData["chains"]) {
        int start = block->pieces[pieceIdx].poly.addChain(glm::vec2((float)chain[0], (float)chain[1]));
        int v = start;
        for(int i = 2; i < chain.size(); i += 2) {
            v = block->pieces[pieceIdx].poly.addPoint(glm::vec2((float)chain[i], (float)chain[i + 1]), v);
        }
        block->pieces[pieceIdx].poly.closeChain(start, v);
    }
}

void loadLevel(const char* name) {

    char path[512];
    sprintf(path, "res/levels/%s.json", name);
    std::ifstream file(path);
    json level = json::parse(file); 

    Arr<Block*, false> addedBlocks;
    addedBlocks.init();
    for(json& blockData : level["blocks"]) {
        Block* block = ObjList<Block>::spawn();
        block->init((bool)blockData["dynamic"]);
        for(json& pieceData : blockData["pieces"]) {
            if(pieceData.contains("model")) {
                loadModelPiece(pieceData, block);
            } else {
                loadPolyPiece(pieceData, block);
            }
        }
        addedBlocks.add(block);
        block->updateMesh();
    }

    for(json& jointData : level["joints"]) {

        if(jointData["type"] == "bolt") {
            Block* a = addedBlocks[(int)jointData["a"]];
            Block* b = addedBlocks[(int)jointData["b"]];
            int aPieceIdx = (int)jointData["aPieceIdx"];
            int bPieceIdx = (int)jointData["bPieceIdx"];
            b2Vec2 pos = b2Vec2((float)jointData["position"][0], (float)jointData["position"][1]);
            Bolt* bolt = ObjList<Bolt>::spawn();
            bolt->init(a, aPieceIdx, b, bPieceIdx, pos);
        }

    }

}
