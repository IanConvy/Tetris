#ifndef INPUTS
#define INPUTS

#include <vector>
#include <map>
#include <string>

#include "GL/glew.h"
#include "GLFW/glfw3.h"

class InputHandler
{
    public:

    InputHandler(GLFWwindow* window);
    std::string getState(std::string keyName);
    std::vector<double> getMousePos();
    static void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mousePosCallBack(GLFWwindow* window, double xpos, double ypos);
    static void mouseClickCallBack(GLFWwindow* window, int button, int action, int mods);

    private:

    std::vector<double> mousePos;
    std::map<const int, bool> prevQueried;
    std::map<const int, int> actionMap;

    void (*keyCallPtr)(GLFWwindow* window, int key, int scancode, int action, int mods);
    void (*mousePosCallPtr)(GLFWwindow* window, double xpos, double ypos);
    void (*mouseClickCallPtr)(GLFWwindow* window, int button, int actions, int mods);

    void keyParser(int key, int action);
    void mouseClickParser(int button, int action);
    void setMousePos(double xpos, double ypos);
    
}; 

extern const std::map<const std::string, const int> keyToInt; 

#endif