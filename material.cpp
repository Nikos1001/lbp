
#include "material.h"
#include "util.h"

Material materials[] = {
    (Material){"green-fabric", 0.5f, FLAT, 0.05f, 0.0f},
    (Material){"factory-metal", 2.5f, SQUARE_BEVEL, 0.05f, 0.05f}
};

int nMats;

void loadMaterials() {
    nMats = sizeof(materials) / sizeof(Material);
    char buf[512];
    for(int i = 0; i < nMats; i++) {
        sprintf(buf, "res/mats/%s/col.jpg", materials[i].name);
        materials[i].col.init();
        materials[i].col.loadFromFile(buf);
        sprintf(buf, "res/mats/%s/norm.jpg", materials[i].name);
        materials[i].norm.init();
        materials[i].norm.loadFromFile(buf);
        sprintf(buf, "res/mats/%s/arm.jpg", materials[i].name);
        materials[i].arm.init();
        materials[i].arm.loadFromFile(buf);
    }
}
