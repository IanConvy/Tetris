#define GLEW_STATIC

#include <string>
#include <iostream>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "../graphics/headers/drawer.hpp"
#include "headers/inputs.hpp"
#include "headers/nes.hpp"
#include "headers/pointclick.hpp"

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
        int height = 899; 
        int width = 1035;
        GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL", nullptr, nullptr);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwMakeContextCurrent(window);
        glewExperimental = GL_TRUE;
        glewInit();

        std::string location = argv[1];
        std::string mode = (argc > 2) ? argv[2] : std::string("nes");
        const int startLevel = (argc > 3) ? std::stoi(argv[3]) : 0;

        InputHandler inputs{window};
        BoardDrawer drawer{location};

        if (mode == std::string("nes")) {
            NESTetris game{startLevel};
            game.assignInputs(inputs.pressed);
            drawer.assignGrid(game.grid);
            drawer.assignLevel(game.dynamic["level"]);
            drawer.assignLineCount(game.dynamic["lineCount"]);
            drawer.assignlineTypeCount(game.lineTypeCount);
            drawer.assignNextPiece(game.nextPiece);
            drawer.assignScore(game.dynamic["score"]);

            double engTime = 0;
            double rendTime = 0;
            const double engSecs = 1 / 60.1;
            const double rendSecs = 1 / 60.1;
            
            while (!glfwWindowShouldClose(window)) {
                double newTime = glfwGetTime();
                if (newTime - engTime >= engSecs) {
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
        else if (mode == std::string("pointclick")) {
            int height = 0, width = 0;
            PointClick game{18, 390, 710, 805, 159};
            game.assignPressed(inputs.pressed);
            game.assignMousePos(inputs.mousePos);
            drawer.assignGrid(game.displayGrid);
            drawer.assignLevel(game.dynamic["level"]);
            drawer.assignLineCount(game.board.lineCount);
            drawer.assignlineTypeCount(game.board.lineTypeCount);
            drawer.assignNextPiece(game.nextPiece);
            drawer.assignScore(game.dynamic["score"]);
            drawer.assignMiscData(game.eval);

            double rendTime = 0;
            const double rendSecs = 1 / 60.1;
            
            while (!glfwWindowShouldClose(window)) {
                double newTime = glfwGetTime();
                if (newTime - rendTime >= rendSecs) {
                    glfwGetWindowSize(window, &width, &height);
                    game.runFrame();
                    drawer.drawFrame();
                    glfwSwapBuffers(window);
                    rendTime = newTime;
                }
                glfwPollEvents();
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