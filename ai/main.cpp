#define GLEW_STATIC

#include <iostream>
#include <vector>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "../game/headers/drawer.hpp"
#include "headers/player.hpp"
#include "headers/tools.hpp"

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

        Player player;
        BoardDrawer drawer{location};
        drawer.assignGrid(player.grid);
        drawer.assignLineCount(player.lineCount);
        drawer.assignNextPiece(player.currPiece);
        drawer.assignlineTypeCount(player.lineTypeCount);
        bool held = false;
        drawer.drawFrame();
        glfwSwapBuffers(window);
        bool topout = false;
        while (!glfwWindowShouldClose(window) && !topout) {
            glfwPollEvents();
            if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS && held == false) {
                held = true;
                topout = player.bestMove();
                drawer.drawFrame();
                glfwSwapBuffers(window);
            }
            else if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE) {
                held = false;
            }
        }
    }    
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
