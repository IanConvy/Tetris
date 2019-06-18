#define GLEW_STATIC

#include <string>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "headers/drawer.hpp"
#include "headers/inputs.hpp"
#include "headers/nes.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main(int argc, char* argv[])
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwSwapInterval(1);

    { // OpenGL scope
        int width = 899; 
        int height = 1035;
        GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL", nullptr, nullptr);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwMakeContextCurrent(window);
        glewExperimental = GL_TRUE;
        glewInit();

        std::string location = argv[1];
        const int startLevel = (argc > 1) ? std::stoi(argv[2]) : 0;

        NESTetris game{startLevel};
        BoardDrawer drawer{location, game};
        InputHandler inputs{window, game.commands};

        double engTime = 0;
        double rendTime = 0;
        const double engSecs = 1 / 60.1;
        const double rendSecs = 1 / 60.1;
        
        while (!glfwWindowShouldClose(window)) {
            double newTime = glfwGetTime();
            if (newTime - engTime >= engSecs) {
                inputs.setCommands();
                game.runFrame();
                engTime = newTime;  
            }
            if (newTime - rendTime >= rendSecs) {
                drawer.drawFrame();
                glfwSwapBuffers(window);
                rendTime = newTime;
            }
            glfwPollEvents();
        }
    }    
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}