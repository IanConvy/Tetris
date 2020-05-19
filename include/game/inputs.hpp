#ifndef INPUTS
#define INPUTS

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <vector>
#include <map>
#include <string>


class InputHandler
{
    public:

    InputHandler(GLFWwindow* window);
    std::map<const std::string, std::string> getStates(std::vector<std::string> keyName);
    std::vector<double> getMousePos();
    std::vector<int> getWindowSize();
    static void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mousePosCallBack(GLFWwindow* window, double xpos, double ypos);
    static void mouseClickCallBack(GLFWwindow* window, int button, int action, int mods);
    static void windowResizeCallBack(GLFWwindow* window, int width, int height);

    private:

    int windowHeight, windowWidth;
    std::vector<double> mousePos;
    std::map<const int, bool> prevQueried;
    std::map<const int, int> actionMap;

    void (*keyCallPtr)(GLFWwindow* window, int key, int scancode, int action, int mods);
    void (*mousePosCallPtr)(GLFWwindow* window, double xpos, double ypos);
    void (*mouseClickCallPtr)(GLFWwindow* window, int button, int actions, int mods);
    void (*windowResizeCallPtr)(GLFWwindow* window, int width, int height);

    void keyParser(int key, int action);
    void mouseClickParser(int button, int action);
    void setMousePos(double xpos, double ypos);
    void resizeWindow(int newHeight, int newWidth);
    
}; 

extern const std::map<const std::string, const int> keyToInt; 

#endif