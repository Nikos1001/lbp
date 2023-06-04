
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
