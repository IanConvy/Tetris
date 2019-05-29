#define GLEW_STATIC

#include "headers/inputs.hpp"

#include <vector>
#include <map>
#include <string>

#include "GL/glew.h"
#include "GLFW/glfw3.h"

InputHandler::InputHandler(GLFWwindow* window, std::map<const std::string, bool>& commands) :
keyPress{0, 0, 0, 0, 0},
callPtr{keyCallBack},
flags{
    {"aHeld", false},
    {"sHeld", false},
    {"leftHeld", false},
    {"rightHeld", false},
    {"downHeld", false}},
commands{commands}
{
    glfwSetKeyCallback(window, callPtr);
    glfwSetWindowUserPointer(window, this);
}

void InputHandler::keyParser(int key,int action)
{
    if (key == GLFW_KEY_A) {
        if (action == GLFW_PRESS) {
            if (!flags["aHeld"] && !flags["sHeld"]) {
                flags["aHeld"] = true;
                commands["doCCW"] = true;
            }
        }  
        if (action == GLFW_RELEASE) {
            flags["aHeld"] = false;
        }
    }
    if (key == GLFW_KEY_S) {
        if (action == GLFW_PRESS) {
            if (!flags["sHeld"] && !flags["aHeld"]) {
                flags["sHeld"] = true;
                commands["doCW"] = true;
            }
        }
        if (action == GLFW_RELEASE) {
            flags["sHeld"] = false;
        }
    }
    if (key == GLFW_KEY_LEFT) {
        if (action == GLFW_PRESS) {
            if (!flags["leftHeld"] && !flags["rightHeld"]) {
                flags["leftHeld"] = true;
                commands["doLeft"] = true;
                commands["clearDAS"] = true;
            }
            if (flags["leftHeld"]) {
                commands["leftDAS"];
            }
        }
        if (action == GLFW_RELEASE) {
            flags["leftHeld"] = false;
        }
    }
    if (key == GLFW_KEY_RIGHT) {
        if (action == GLFW_PRESS) {
            if (!flags["rightHeld"] && !flags["leftHeld"]) {
                flags["rightHeld"] = true;
                commands["doRight"] = true;
                commands["clearDAS"] = true;
            }
            if (flags["rightHeld"]) {
                commands["rightDAS"];
            }
        }
        if (action == GLFW_RELEASE) {
            flags["rightHeld"] = false;
        }
    }
    if (key == GLFW_KEY_DOWN) {
        if (action == GLFW_PRESS) {
            commands["softDrop"] = true;
        }
        if (action == GLFW_RELEASE) {
            commands["softDrop"] = false;
        }
    }
}

void InputHandler::keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    static_cast<InputHandler*>(glfwGetWindowUserPointer(window))->keyParser(key, action);
}




