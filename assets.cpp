
#include "assets.h"

void loadAssets() {
    Mesh boltMesh_;
    boltMesh_.loadFrom("res/models/bolt.blend");
    boltMesh = addResource<Mesh>(boltMesh_); 
    Texture boltCol_;
    boltCol_.init();
    boltCol_.loadFromFile("res/tex/boltCol.png");
    boltCol = addResource<Texture>(boltCol_);
    Texture boltNorm_;
    boltNorm_.init();
    boltNorm_.loadFromFile("res/tex/boltNorm.png");
    boltNorm = addResource<Texture>(boltNorm_);
    Texture boltArm_;
    boltArm_.init();
    boltArm_.loadFromFile("res/tex/boltArm.png");
    boltArm = addResource<Texture>(boltArm_);
}

ResHandle<Mesh> boltMesh;
ResHandle<Texture> boltCol;
ResHandle<Texture> boltNorm;
ResHandle<Texture> boltArm;