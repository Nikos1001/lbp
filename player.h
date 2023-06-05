
#ifndef PLAYER_H
#define PLAYER_H

#include "box2d.h"
#include "input.h"
#include "util.h"

struct PlayerLimb {
    b2Body* body;
    b2Fixture* solid;
    b2Fixture* backSensor;
    b2Fixture* frontSensor;
};

class Player {

public:

    void init(float x, float y);
    void free();
    void render();
    void update(float dt);
    void applyForce(glm::vec2 force);

    int layer;

private:

    void kill();
    
    PlayerLimb head;
    PlayerLimb legs;

    ActionBuffer jumpBuffer;
    bool rising;

};

#endif
