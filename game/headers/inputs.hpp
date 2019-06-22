#define GLEW_STATIC
#ifndef INPUTS
#define INPUTS

#include <vector>
#include <map>
#include <string>

#include "GL/glew.h"
#include "GLFW/glfw3.h"

struct InputHandler
{
    std::vector<bool> keyPress;
    void (*callPtr)(GLFWwindow* window, int key, int scancode, int action, int mods);
    std::map<const std::string, bool> pressed, flags;

    InputHandler(GLFWwindow* window);
    void keyParser(int key, int action);
    static void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);
};

#endif