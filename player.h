
#ifndef PLAYER_H
#define PLAYER_H

#include "box2d.h"
#include "input.h"
#include "common/util.h"

class CapsuleFixtures {

public:
    void init(b2Body* body, bool sensor);
    void setLayer(int layer);
    bool containsFixture(b2Fixture* fixture);
    b2Fixture* head;
    b2Fixture* torso;
    b2Fixture* legs;

};

struct GrabPoint {
    b2Body* body;
    b2Vec2 point;
    float dist;
    bool exists;
};

class Player {

public:

    void init(float x, float y, int layer);
    void free();
    void render();
    void update(float dt);
    void applyForce(glm::vec2 force);


private:

    void kill();

    b2Body* body;
    CapsuleFixtures solid;
    CapsuleFixtures crushSensor;
    CapsuleFixtures frontSensor;
    CapsuleFixtures backSensor;
    b2Fixture* grabSensor;

    float prevAngle;
    
    ActionBuffer jumpBuffer;
    bool rising;

    int layer;

    bool grabbing;
    b2Body* grabOther;
    b2DistanceJoint* grabJoint;
    GrabPoint getGrabPoint();

};

#endif
