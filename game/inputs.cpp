#include "headers/inputs.hpp"

#include <vector>
#include <map>
#include <string>
#include <iostream>

#include "GL/glew.h"
#include "GLFW/glfw3.h"

InputHandler::InputHandler(GLFWwindow* window) :
/*
* The InputHandler class records mouse and keyboard events in a game-agnostic manner. The 
* format of the class is simply a set of containers to hold the input data and a set of
* functions to retrieve the input events from GLFW. The callback functions are static 
* members passed to GLFW via pointer in order to capture the callback response.
*/
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
    {"space", false},
    {"mouseLeft", false},
    {"mouseRight", false}}
{
    // These functions simply pass the callback pointers to GLFW
    glfwSetKeyCallback(window, keyCallPtr);
    glfwSetCursorPosCallback(window, mousePosCallPtr);
    glfwSetMouseButtonCallback(window, mouseClickCallPtr);

    // This function allows GLFW to access members of this class instance
    glfwSetWindowUserPointer(window, this);
}

void InputHandler::keyParser(int key, int action)
/*
* This function is called by the callback members and uses the reported 
* action (pressed or released) to flip the boolean in the key map member
* which reports whether a given keyboard key is being pressed.
*/
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
    if (key == GLFW_KEY_SPACE) {
        if (action == GLFW_PRESS) {
            pressed["space"] = true;
        }
        if (action == GLFW_RELEASE) {
            pressed["space"] = false;
        }
    }
}

void InputHandler::mouseClickParser(int button, int action)
/*
* This function works the same as keyParser, except it is called for 
* mouse button presses. Note that the if-else structure is used here 
* but not in keyParser because a mouse button action is either GLFW_PRESS 
* or GLFW_RELEASE, whereas a keyboard key also has the GLFW_REPEAT action.
*/
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
/*
* This function simply records the x-y position it 
* recieves from the mouse movement callback.
*/
{
    mousePos[0] = xpos;
    mousePos[1] = ypos;
}

/*
* The following three functions are passed to GLFW for it to call whenever a keyboard or
* mouse event is detected. The function arguments are set by the GLFW API, while the
* body is custom. GLFW is only able to access our program's variables through a single 
* pointer that was set in the InputHandler constructor. This pointer is returned by the
* glfwGetWindowUserPointer function and must be cast back into InputHandler*. After casting,
* the pointer can be used to access the InputHandler member functions.
*/

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




