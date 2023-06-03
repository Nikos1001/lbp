
#include "input.h"
#include "main.h"

bool keyDown(int key) {
    return glfwGetKey(rnd.window, key) == GLFW_PRESS; 
}
