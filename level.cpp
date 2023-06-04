
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

    for(json& poly : level["poly"]) {
        Block* block = blocks.spawn();
        block->init(glm::vec2(0.0f, 0.0f));
        block->setDynamic((bool)poly["dynamic"]);
        block->frontLayer = (int)poly["front"];
        block->backLayer = (int)poly["back"];
        for(json& chain : poly["chains"]) {
            int start = block->poly.addChain(glm::vec2((float)chain[0], (float)chain[1]));
            int v = start;
            for(int i = 2; i < chain.size(); i += 2) {
                v = block->poly.addPoint(glm::vec2((float)chain[i], (float)chain[i + 1]), v);
            }
            block->poly.closeChain(start, v);
        }
        for(int i = 0; i < nMats; i++) {
            if(strcmp(materials[i].name, ((std::string)poly["material"]).c_str()) == 0) {
                block->material = i;
            }
        }
        block->updateMesh();
    }

}
