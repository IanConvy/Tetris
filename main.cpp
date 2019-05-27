#define GLEW_STATIC

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "headers/board_drawer.hpp"
#include "headers/inputs.hpp"
#include "headers/pieces.hpp"

#include <iostream>
#include<vector>

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

    { // OpenGL scope
        GLFWwindow* window = initialize_window(899, 1035);

        BoardDrawer drawer{};
        InputHandler inputs{window};
        Piece piece{sqPiece};
        piece.setPosition(5, 5, 0);

        double prev_time = 0;
        double frameSecs = 1 / 1.5;
        while (!glfwWindowShouldClose(window)) {
            double new_time = glfwGetTime();
            double timeDiff = new_time - prev_time;
            if (timeDiff >= frameSecs) {
                // std::cout << 1 / timeDiff << std::endl;
                std::vector<bool> keyPress = inputs.getKeyPress();
                if (keyPress[3]) {
                    piece.rotate(1);
                }
                drawer.drawBoard();
                for (int i = 0; i < 4; ++i) {
                    drawer.drawBlock(piece.rows[i], piece.cols[i]);
                    std::cout << piece.rows[i] << " " << piece.cols[i] << std::endl;
                }
                glfwSwapBuffers(window);
                glfwPollEvents();
                prev_time = new_time;
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