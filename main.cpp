#define GLEW_STATIC

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "headers/board_drawer.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
GLFWwindow* initialize_window(unsigned int height, unsigned int width); 

int main()
{
    GLFWwindow* window = initialize_window(899, 1035);

    while (!glfwWindowShouldClose(window)) {
        BoardDrawer drawer{};
        processInput(window);
        drawer.drawBoard();
        for (int i = 0; i < 20; ++i) {
            drawer.drawBlock(i, 0);
        }
        for (int i = 0; i < 10; ++i) {
            drawer.drawBlock(0, i);
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
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
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL", nullptr, nullptr);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    glewInit();
    return window;
}