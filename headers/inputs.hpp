#define GLEW_STATIC
#ifndef INPUTS
#define INPUTS

#include<vector>

#include "GL/glew.h"
#include "GLFW/glfw3.h"

struct InputHandler
{
    std::vector<bool> keyPress;
    void (*callPtr)(GLFWwindow* window, int key, int scancode, int action, int mods);

    InputHandler(GLFWwindow* window);
    void keyParser(int key, int action);
    static void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);
    std::vector<bool> getKeyPress();
};
#endif