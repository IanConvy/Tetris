#define GLEW_STATIC

#include <iostream>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "headers/drawer.hpp"
#include "headers/inputs.hpp"
#include "headers/pieces.hpp"
#include "headers/nes.hpp"
#include "headers/text.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
GLFWwindow* initialize_window(unsigned int height, unsigned int width); 

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwSwapInterval(1);

    { // OpenGL scope
        GLFWwindow* window = initialize_window(899, 1035);

        BoardDrawer drawer{};
        NESTetris game{};
        InputHandler inputs{window, game.commands};

        double engTime = 0;
        double rendTime = 0;
        double engSecs = 1 / 60.1;
        double rendSecs = 1 / 60.1;
        
        while (!glfwWindowShouldClose(window)) {
            double newTime = glfwGetTime();
            if (newTime - engTime >= engSecs) {
                inputs.setCommands();
                game.runFrame();
                engTime = newTime;  
            }
            if (newTime - rendTime >= rendSecs) {
                auto fillBlocks = game.grid.getFilledBlocks();
                drawer.drawBoard();
                drawer.drawPieceBlocks(fillBlocks);
                drawer.drawPreview(game.nextPiece->data);
                drawer.drawLineCount("lines-123");
                drawer.drawScore("567421");
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

GLFWwindow* initialize_window(unsigned int height, unsigned int width)
{
    GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL", nullptr, nullptr);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    glewInit();
    return window;
}