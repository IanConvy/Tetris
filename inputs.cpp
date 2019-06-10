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
    {"escHeld", false}},
commands{commands}
{
    glfwSetKeyCallback(window, callPtr);
    glfwSetWindowUserPointer(window, this);
}

void InputHandler::setCommands()
{   // A key:
    if (pressed["a"] && !flags["aHeld"] && !flags["sHeld"]) {
        commands["doCCW"] = true;
        flags["aHeld"] = true;
    }
    if (!pressed["a"]) {
        flags["aHeld"] = false;
    }
    // S key:
    if (pressed["s"] && !flags["sHeld"] && !flags["aHeld"]) {
        commands["doCW"] = true;
        flags["sHeld"] = true;
    }
    if (!pressed["s"]) {
        flags["sHeld"] = false;
    }
    // Left key:
    if (pressed["left"] && flags["leftHeld"]) { // This being first is important to avoid double move.
            commands["leftDAS"] = true;
        }
    if (pressed["left"] && !flags["leftHeld"] && !flags["rightHeld"]) {
        flags["leftHeld"] = true;
        commands["doLeft"] = true;
        commands["clearDAS"] = true;
    }    
    if (!pressed["left"]) {
        flags["leftHeld"] = false;
    }
    // Right key:
    if (pressed["right"] && flags["rightHeld"]) { // This being first is important to avoid double move.
        commands["rightDAS"] = true;
    }
    if (pressed["right"] && !flags["rightHeld"] && !flags["leftHeld"]) {
        flags["rightHeld"] = true;
        commands["doRight"] = true;
        commands["clearDAS"] = true;
    }    
    if (!pressed["right"]) {
        flags["rightHeld"] = false;
    }
    // Down key:
    if (pressed["down"]) {
        commands["softDrop"] = true;
    }
    else {
        commands["softDrop"] = false;
    }
    // Escape key:
    if (pressed["esc"]) {
        if (!flags["escHeld"]) {
            commands["reset"] = true;
            flags["escHeld"] = true;
        }
    }
    else {
        flags["escHeld"] = false;
    }
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




