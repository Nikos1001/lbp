
#ifndef PLAYER_H
#define PLAYER_H

#include "box2d.h"

class Player {

public:

    void init(float x, float y);
    void free();
    void render();
    void update(float dt);

private:
    
    b2Body* head;
    b2Body* legs;

};

#endif
