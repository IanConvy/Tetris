#define GLEW_STATIC

#include "headers/inputs.hpp"

#include <vector>
#include <map>
#include <string>

#include "GL/glew.h"
#include "GLFW/glfw3.h"

InputHandler::InputHandler(GLFWwindow* window) :
keyPress{0, 0, 0, 0, 0},
callPtr{keyCallBack},
pressed{
    {"a", false},
    {"s", false},
    {"lef", false},
    {"right", false},
    {"down", false},
    {"esc", false}},
flags{
    {"aHeld", false},
    {"sHeld", false},
    {"leftHeld", false},
    {"rightHeld", false},
    {"escHeld", false}}
{
    glfwSetKeyCallback(window, callPtr);
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
    if (key == GLFW_KEY_ESCAPE) {
        if (action == GLFW_PRESS) {
            pressed["esc"] = true;
        }
        if (action == GLFW_RELEASE) {
            pressed["esc"] = false;
        }
    }
}

void InputHandler::keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    static_cast<InputHandler*>(glfwGetWindowUserPointer(window))->keyParser(key, action);
}




