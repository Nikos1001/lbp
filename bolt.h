
#ifndef BOLT_H
#define BOLT_H

#include "block.h"

class Bolt {

public:
    
    void init(Block* a, int aPieceIdx, Block* b, int bPieceIdx, b2Vec2 pos);
    void render();
    void free();

private:

    b2RevoluteJoint* joint;

    Block* a;
    int aPieceIdx;
    Block* b;
    int bPieceIdx;

};

#endif
