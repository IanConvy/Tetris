#define GLEW_STATIC

#include "headers/inputs.hpp"

#include<vector>

#include "GL/glew.h"
#include "GLFW/glfw3.h"

InputHandler::InputHandler(GLFWwindow* window) :
keyPress{0, 0, 0, 0, 0},
callPtr{keyCallBack}
{
    glfwSetKeyCallback(window, callPtr);
    glfwSetWindowUserPointer(window, this);
}

std::vector<bool> InputHandler::getKeyPress()
{
    return keyPress;
}

void InputHandler::keyParser(int key,int action)
{
    if (key == GLFW_KEY_LEFT) {
        if (action == GLFW_PRESS) {
            keyPress[0] = true;
        }
        if (action == GLFW_RELEASE) {
            keyPress[0] = false;
        }
    }
    if (key == GLFW_KEY_RIGHT) {
        if (action == GLFW_PRESS) {
            keyPress[1] = true;
        }
        if (action == GLFW_RELEASE) {
            keyPress[1] = false;
        }
    }
    if (key == GLFW_KEY_DOWN) {
        if (action == GLFW_PRESS) {
            keyPress[2] = true;
        }
        if (action == GLFW_RELEASE) {
            keyPress[2] = false;
        }
    }
    if (key == GLFW_KEY_A) {
        if (action == GLFW_PRESS) {
            keyPress[3] = true;
        }
        if (action == GLFW_RELEASE) {
            keyPress[3] = false;
        }
    }
    if (key == GLFW_KEY_S) {
        if (action == GLFW_PRESS) {
            keyPress[4] = true;
        }
        if (action == GLFW_RELEASE) {
            keyPress[4] = false;
        }
    }
}

void InputHandler::keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    static_cast<InputHandler*>(glfwGetWindowUserPointer(window))->keyParser(key, action);
}




