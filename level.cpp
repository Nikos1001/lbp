
#include "level.h"
#include "main.h"
#include "util.h"
#include "material.h"

#include <fstream>
using json = nlohmann::json;

void loadLevel(const char* name) {

    char path[512];
    sprintf(path, "res/levels/%s.json", name);
    std::ifstream file(path);
    json level = json::parse(file); 

    for(json& blockData : level["blocks"]) {
        Block* block = blocks.spawn();
        block->init((bool)blockData["dynamic"]);
        for(json& pieceData : blockData["pieces"]) {
            std::string materialName = (std::string)pieceData["material"];
            const char* materialNameCStr = materialName.c_str();
            int material = 0;
            for(int i = 0; i < nMats; i++)
                if(strcmp(materials[i].name, materialNameCStr) == 0)
                    material = i;
            int pieceIdx = block->addPiece((int)pieceData["front"], (int)pieceData["back"], material);
            for(json& chain : pieceData["chains"]) {
                int start = block->pieces[pieceIdx].poly.addChain(glm::vec2((float)chain[0], (float)chain[1]));
                int v = start;
                for(int i = 2; i < chain.size(); i += 2) {
                    v = block->pieces[pieceIdx].poly.addPoint(glm::vec2((float)chain[i], (float)chain[i + 1]), v);
                }
                block->pieces[pieceIdx].poly.closeChain(start, v);
            }
        }
        block->updateMesh();
    }

}
