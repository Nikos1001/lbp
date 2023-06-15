
#include "material.h"
#include "util.h"

Material materials[] = {
    (Material){"green-fabric", 0.5f, FLAT, 0.05f, 0.0f},
    (Material){"factory-metal", 2.5f, SQUARE_BEVEL, 0.05f, 0.05f}
};

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
    (ModelMaterial){"starlight", 0.35f, 10, starlightVerts}
};

int nMats;
int nModelMats;

void loadMaterials() {
    char buf[512];

    nMats = sizeof(materials) / sizeof(Material);
    for(int i = 0; i < nMats; i++) {
        sprintf(buf, "res/mats/%s/col.jpg", materials[i].name);
        materials[i].col.init();
        materials[i].col.loadFromFile(buf);
        sprintf(buf, "res/mats/%s/norm.jpg", materials[i].name);
        materials[i].norm.init();
        materials[i].norm.loadFromFile(buf);
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

        modelMaterials[i].polygon.init();
        int start = modelMaterials[i].polygon.addChain(glm::vec2(modelMaterials[i].colliderVerts[0], modelMaterials[i].colliderVerts[1]));
        int v = start;
        for(int j = 1; j < modelMaterials[i].nColliderVerts; j++) {
            v = modelMaterials[i].polygon.addPoint(glm::vec2(modelMaterials[i].colliderVerts[2 * j + 0], modelMaterials[i].colliderVerts[2 * j + 1]), v);
        }
        modelMaterials[i].polygon.closeChain(start, v);
    }


}
