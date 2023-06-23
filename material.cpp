
#include "material.h"
#include "util.h"
#include "lib/json/single_include/nlohmann/json.hpp"
using json = nlohmann::json;

Arr<Material, true> materials;

float starlightVerts[] = {
     0.0f,      0.43774f,
     0.15866f,  0.21837f,
     0.41631f,  0.13527f,
     0.25671f, -0.083412f,
     0.2573f,  -0.35414f,
     0.0f,     -0.26993f,
    -0.2573f,  -0.35414f,
    -0.25671f, -0.083412f,
    -0.41631f,  0.13527f,
    -0.15866f,  0.21837f
};

ModelMaterial modelMaterials[] = {
    (ModelMaterial){"starlight", 0.35f, 10, starlightVerts, 10.0f}
};

int nModelMats;

void loadMaterials() {
    char buf[512];

    std::ifstream file("res/mats/data.json");
    json materialsData = json::parse(file);

    materials.init();
    
    for(json& materialData : materialsData["materials"]) {
        Material mat;
        strcpy(mat.name, ((std::string)materialData["name"]).c_str());

        mat.density = (float)materialData["density"];
        mat.bevelWidth = (float)materialData["bevelWidth"];
        mat.faceInset = (float)materialData["faceInset"];
        mat.uvScale = (float)materialData["uvScale"];
        mat.meshGen = FLAT;
        if((std::string)(materialData["bevelType"]) == "square")
            mat.meshGen = SQUARE_BEVEL;
        mat.normStrength = 1.0f;

        sprintf(buf, "res/mats/%s/face-col.png", mat.name);
        mat.col[0].init();
        mat.col[0].loadFromFile(buf);
        sprintf(buf, "res/mats/%s/face-norm.png", mat.name);
        mat.norm[0].init();
        mat.norm[0].loadFromFile(buf);
        sprintf(buf, "res/mats/%s/face-arm.png", mat.name);
        mat.arm[0].init();
        mat.arm[0].loadFromFile(buf);

        sprintf(buf, "res/mats/%s/bevel-col.png", mat.name);
        mat.col[1].init();
        mat.col[1].loadFromFile(buf);
        sprintf(buf, "res/mats/%s/bevel-norm.png", mat.name);
        mat.norm[1].init();
        mat.norm[1].loadFromFile(buf);
        sprintf(buf, "res/mats/%s/bevel-arm.png", mat.name);
        mat.arm[1].init();
        mat.arm[1].loadFromFile(buf);

        sprintf(buf, "res/mats/%s/border-col.png", mat.name);
        mat.col[2].init();
        mat.col[2].loadFromFile(buf);
        sprintf(buf, "res/mats/%s/border-norm.png", mat.name);
        mat.norm[2].init();
        mat.norm[2].loadFromFile(buf);
        sprintf(buf, "res/mats/%s/border-arm.png", mat.name);
        mat.arm[2].init();
        mat.arm[2].loadFromFile(buf);

        materials.add(mat);
    }

    nModelMats = sizeof(modelMaterials) / sizeof(ModelMaterial);
    for(int i = 0; i < nModelMats; i++) {
        sprintf(buf, "res/model-mats/%s/model.blend", modelMaterials[i].name);
        modelMaterials[i].mesh.loadFrom(buf);
        sprintf(buf, "res/model-mats/%s/col.jpg", modelMaterials[i].name);
        modelMaterials[i].col.init();
        modelMaterials[i].col.loadFromFile(buf);
        sprintf(buf, "res/model-mats/%s/norm.jpg", modelMaterials[i].name);
        modelMaterials[i].norm.init();
        modelMaterials[i].norm.loadFromFile(buf);
        sprintf(buf, "res/model-mats/%s/arm.jpg", modelMaterials[i].name);
        modelMaterials[i].arm.init();
        modelMaterials[i].arm.loadFromFile(buf);

        modelMaterials[i].polygon.init();
        int start = modelMaterials[i].polygon.addChain(glm::vec2(modelMaterials[i].colliderVerts[0], modelMaterials[i].colliderVerts[1]));
        int v = start;
        for(int j = 1; j < modelMaterials[i].nColliderVerts; j++) {
            v = modelMaterials[i].polygon.addPoint(glm::vec2(modelMaterials[i].colliderVerts[2 * j + 0], modelMaterials[i].colliderVerts[2 * j + 1]), v);
        }
        modelMaterials[i].polygon.closeChain(start, v);
    }


}
