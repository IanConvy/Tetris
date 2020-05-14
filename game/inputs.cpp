#include "headers/inputs.hpp"

#include <vector>
#include <map>
#include <string>
#include <iostream>

#include "GL/glew.h"
#include "GLFW/glfw3.h"

const std::map<const std::string, const int> keyToInt{
    {"a", GLFW_KEY_A},
    {"s", GLFW_KEY_S},
    {"z", GLFW_KEY_Z},
    {"x", GLFW_KEY_X},
    {"left", GLFW_KEY_LEFT},
    {"right", GLFW_KEY_RIGHT},
    {"down", GLFW_KEY_DOWN},
    {"up", GLFW_KEY_UP},
    {"esc", GLFW_KEY_ESCAPE},
    {"space", GLFW_KEY_SPACE},
    {"mouseLeft", GLFW_MOUSE_BUTTON_LEFT},
    {"mouseRight", GLFW_MOUSE_BUTTON_RIGHT}}; 

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
prevQueried{ // Records whether the state of a button has previously been queried while pressed
    {GLFW_KEY_A, false},
    {GLFW_KEY_S, false},
    {GLFW_KEY_Z, false},
    {GLFW_KEY_X, false},
    {GLFW_KEY_LEFT, false},
    {GLFW_KEY_RIGHT, false},
    {GLFW_KEY_DOWN, false},
    {GLFW_KEY_UP, false},
    {GLFW_KEY_ESCAPE, false},
    {GLFW_KEY_SPACE, false},
    {GLFW_MOUSE_BUTTON_LEFT, false},
    {GLFW_MOUSE_BUTTON_RIGHT, false}},
actionMap{ // Records the current state of the key as reported by the hardware
    {GLFW_KEY_A, 0},
    {GLFW_KEY_S, 0},
    {GLFW_KEY_Z, 0},
    {GLFW_KEY_X, 0},
    {GLFW_KEY_LEFT, 0},
    {GLFW_KEY_RIGHT, 0},
    {GLFW_KEY_DOWN,0},
    {GLFW_KEY_UP, 0},
    {GLFW_KEY_ESCAPE, 0},
    {GLFW_KEY_SPACE, 0},
    {GLFW_MOUSE_BUTTON_LEFT, 0},
    {GLFW_MOUSE_BUTTON_RIGHT, 0}}
{
    // These functions simply pass the callback pointers to GLFW
    glfwSetKeyCallback(window, keyCallPtr);
    glfwSetCursorPosCallback(window, mousePosCallPtr);
    glfwSetMouseButtonCallback(window, mouseClickCallPtr);

    // This function allows GLFW to access members of this class instance
    glfwSetWindowUserPointer(window, this);
}

std::string InputHandler::getState(std::string keyName) 
/*
 * This function returns the state of the desired button. The states
 * are "off" if the button is not pressed, "pressed" if the button has
 * just been pressed, and "held" if the button held down. It is important
 * to note that the difference between "pressed" and "held" is NOT the same
 * as the difference between GLFW_PRESS and GLFW_REPEAT as reported by
 * GLFW, since keyboards have a delay between reporting that a button
 * has been pressed and reporting that a button is being held down. In 
 * order to make this feedback instantaneous as required, the InputHandler
 * records whether it has been queried regarding a particular key while the 
 * button is being pressed (i.e. while GLFW has registered a press but not 
 * yet a release). If the button had previously been queried, the function
 * returns the state "held" rather than "pressed".
 */ 
{
    auto keyIntItr = keyToInt.find(keyName); 
    if (keyIntItr != keyToInt.end()) { // Only proceed if key name is valid
        int key = keyIntItr->second;
        if (actionMap[key] == GLFW_RELEASE) {
            prevQueried[key] = false;
            return "off";
        }
        else if (actionMap[key] != GLFW_RELEASE) { // True for GLFW_PRESS and GLFW_REPEAT
            if (!prevQueried[key]) {
                prevQueried[key] = true;
                return "pressed";
            }
            else if (prevQueried[key]) {
                return "held";
            }
        }
    }
    else { // If no matching key is found, return empty string to indicate failure
        return "";
    }
}

std::vector<double> InputHandler::getMousePos()
/*
 * This function simply returns a vector containing the current position 
 * of the mouse.
 */
{
    return mousePos;
}

void InputHandler::keyParser(int key, int action)
/*
 * This function is called by the callback members and records the action 
 * associated with the key reported by GLFW. It can be either GLFW_PRESS,
 * GLFW_RELEASE, or GLFW_REPEAT, all of which are mapped to integers. Keys
 * which were not placed in the action map at initialization are not tracked.
 */
{
    if (actionMap.find(key) != actionMap.end()) {
        actionMap[key] = action;
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
    static_cast<InputHandler*>(glfwGetWindowUserPointer(window))->keyParser(button, action);
}




