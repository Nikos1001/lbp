
#ifndef INPUT_H
#define INPUT_H

#include "common/util.h"

void updateInput();
bool keyDown(int key);
bool keyPressed(int key);
glm::vec2 getMousePos();

class ActionBuffer {

public:
    void init(float pre, float post, float cooldown);
    void update(float dt);
    void setActionAvailable(bool available);
    void doAction();
    bool isActionTime();

    float pre;
    float post;
    float cooldown;

private:

    bool actionAvailable;
    float preTimer;
    float postTimer;
    float cooldownTimer;

};

#endif
