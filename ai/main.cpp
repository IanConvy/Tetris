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
void runBulk(float numRuns);
void runDisplay(std::string location);

int main(int argc, char* argv[])
{
    bool display = (argc > 2 && std::string(argv[2]) == std::string("display")) ? true : false;
    float numRuns = (argc > 2 && display == false) ? numRuns = std::stoi(argv[2]) : 100; 

    if (display) {
        runDisplay(argv[1]);
    }
    else {
        runBulk(numRuns);
    }
}

void runBulk(float numRuns)
{
    int totalLineCount = 0;
    std::vector<int> totalLineTypeCount = {0, 0, 0, 0};
    for (int runs = 0; runs < numRuns; ++runs) {
        std::cout << "\r" << runs << std::flush;
        Player player;
        bool topout = false;
        while (!topout) {
            topout = player.bestMove();
        }
        totalLineCount += player.lineCount;
        for (int i = 0; i < player.lineTypeCount.size(); ++i) {
            totalLineTypeCount[i] += player.lineTypeCount[i];
        }
    }
    std::cout << "\nAverage Line Count: " << totalLineCount / numRuns
            << "\n   Single: " << totalLineTypeCount[0] / numRuns
            << "\n   Double: " << totalLineTypeCount[1] / numRuns
            << "\n   Triple: " << totalLineTypeCount[2] / numRuns
            << "\n   Tetris: " << totalLineTypeCount[3] / numRuns
        << std::endl;
}

void runDisplay(std::string location)
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
                player.evaluateGrid(player.grid.grid, true);
                drawer.drawFrame();
                glfwSwapBuffers(window);
            }
            else if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE) {
                held = false;
            }
        }
    }    
    glfwTerminate();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
