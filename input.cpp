
#include "input.h"
#include "main.h"

static bool prevKeyPressed[GLFW_KEY_LAST];

void updateInput() {
    for(int i = 0; i < GLFW_KEY_LAST; i++)
        prevKeyPressed[i] = keyDown(i);
}

bool keyDown(int key) {
    return glfwGetKey(rnd.window, key) == GLFW_PRESS; 
}

bool keyPressed(int key) {
    return keyDown(key) && !prevKeyPressed[key]; 
}


void ActionBuffer::init(float pre, float post, float cooldown) {
    actionAvailable = false;
    this->pre = pre;
    this->post = post;
    this->cooldown = cooldown;
    preTimer = -1;
    postTimer = -1;
    cooldownTimer = -1;
}

void ActionBuffer::update(float dt) {
    preTimer -= dt;
    postTimer -= dt;
    cooldownTimer -= dt;
}

void ActionBuffer::setActionAvailable(bool available) {
    if(actionAvailable && !available) {
        postTimer = post;
    }
    actionAvailable = available;
}

void ActionBuffer::doAction() {
    if(cooldownTimer > 0)
        return;
    preTimer = pre;
    cooldownTimer = cooldown;
}

bool ActionBuffer::isActionTime() {
    bool res = (postTimer > 0 || actionAvailable) && preTimer > 0;
    if(res) {
        preTimer = -1;
        postTimer = -1;
    }
    return res;
}