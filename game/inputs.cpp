#define GLEW_STATIC

#include "headers/inputs.hpp"

#include <vector>
#include <map>
#include <string>
#include <iostream>

#include "GL/glew.h"
#include "GLFW/glfw3.h"

InputHandler::InputHandler(GLFWwindow* window) :
keyPress{0, 0, 0, 0, 0},
mousePos{0, 0},
keyCallPtr{keyCallBack},
mousePosCallPtr{mousePosCallBack},
mouseClickCallPtr{mouseClickCallBack},
pressed{
    {"a", false},
    {"s", false},
    {"z", false},
    {"x", false},
    {"lef", false},
    {"right", false},
    {"down", false},
    {"up", false},
    {"esc", false},
    {"mouseLeft", false},
    {"mouseRight", false}}
{
    glfwSetKeyCallback(window, keyCallPtr);
    glfwSetCursorPosCallback(window, mousePosCallPtr);
    glfwSetMouseButtonCallback(window, mouseClickCallPtr);
    glfwSetWindowUserPointer(window, this);
}

void InputHandler::keyParser(int key,int action)
{
    if (key == GLFW_KEY_A) {
        if (action == GLFW_PRESS) {
            pressed["a"] = true;
        }  
        if (action == GLFW_RELEASE) {
            pressed["a"] = false;
        }
    }
    if (key == GLFW_KEY_S) {
        if (action == GLFW_PRESS) {
            pressed["s"] = true;
        }
        if (action == GLFW_RELEASE) {
            pressed["s"] = false;
        }
    }
    if (key == GLFW_KEY_Z) {
        if (action == GLFW_PRESS) {
            pressed["z"] = true;
        }
        if (action == GLFW_RELEASE) {
            pressed["z"] = false;
        }
    }
    if (key == GLFW_KEY_X) {
        if (action == GLFW_PRESS) {
            pressed["x"] = true;
        }
        if (action == GLFW_RELEASE) {
            pressed["x"] = false;
        }
    }
    if (key == GLFW_KEY_LEFT) {
        if (action == GLFW_PRESS) {
            pressed["left"] = true;
        }
        if (action == GLFW_RELEASE) {
            pressed["left"] = false;
        }
    }
    if (key == GLFW_KEY_RIGHT) {
        if (action == GLFW_PRESS) {
            pressed["right"] = true;
        }
        if (action == GLFW_RELEASE) {
            pressed["right"] = false;
        }
    }
    if (key == GLFW_KEY_DOWN) {
        if (action == GLFW_PRESS) {
            pressed["down"] = true;
        }
        if (action == GLFW_RELEASE) {
            pressed["down"] = false;
        }
    }
    if (key == GLFW_KEY_UP) {
        if (action == GLFW_PRESS) {
            pressed["up"] = true;
        }
        if (action == GLFW_RELEASE) {
            pressed["up"] = false;
        }
    }
    if (key == GLFW_KEY_ESCAPE) {
        if (action == GLFW_PRESS) {
            pressed["esc"] = true;
        }
        if (action == GLFW_RELEASE) {
            pressed["esc"] = false;
        }
    }
}

void InputHandler::mouseClickParser(int button, int action)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            pressed["mouseLeft"] = true;
        }
        else {
            pressed["mouseLeft"] = false;
        }
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            pressed["mouseRight"] = true;
        }
        else {
            pressed["mouseRight"] = false;
        }
    }
}

void InputHandler::setMousePos(double xpos, double ypos)
{
    mousePos[0] = xpos;
    mousePos[1] = ypos;
}

void InputHandler::keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    static_cast<InputHandler*>(glfwGetWindowUserPointer(window))->keyParser(key, action);
}

void InputHandler::mousePosCallBack(GLFWwindow* window, double xpos, double ypos)
{
    static_cast<InputHandler*>(glfwGetWindowUserPointer(window))->setMousePos(xpos, ypos);
}

void InputHandler::mouseClickCallBack(GLFWwindow* window, int button, int action, int mode)
{
    static_cast<InputHandler*>(glfwGetWindowUserPointer(window))->mouseClickParser(button, action);
}




