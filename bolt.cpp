
#include "bolt.h"
#include "main.h"
#include "assets.h"
#include "material.h"

void Bolt::init(Block* a, int aPieceIdx, Block* b, int bPieceIdx, b2Vec2 pos) {
    b2RevoluteJointDef jointDef; 
    jointDef.Initialize(a->body, b->body, pos);
    joint = (b2RevoluteJoint*)phys->CreateJoint(&jointDef);
    this->a = a;
    this->aPieceIdx = aPieceIdx;
    this->b = b;
    this->bPieceIdx = bPieceIdx;
}

void Bolt::render() {
    glm::mat4 trans = glm::mat4(1.0f);
    b2Vec2 jointPos = joint->GetAnchorA(); 
    int layer = std::min(a->pieces[aPieceIdx].frontLayer, b->pieces[bPieceIdx].frontLayer);
    trans = glm::translate(trans, glm::vec3(jointPos.x, jointPos.y, -layer));
    trans = glm::rotate(trans, joint->GetJointAngle(), glm::vec3(0.0f, 0.0f, 1.0f));
    trans = glm::scale(trans, glm::vec3(0.3f));
    rnd.renderMesh(*boltMesh.get(), *boltCol.get(), *boltNorm.get(), *boltArm.get(), trans, 1.0f, 1.0f, 0.0f); 
}

void Bolt::free() {
    phys->DestroyJoint(joint);
}
