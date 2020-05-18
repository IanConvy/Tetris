#define GLEW_STATIC
#include "headers/shader.hpp"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

/*
 * The Shader class, shamelessly ripped off from learnopengl.com, 
 * creates a useful interface for compiling and using shader programs.
 * Its constructor loads and compiles the vertex and fragment shaders
 * from the passed file paths, while its member functions allow the
 * linked shader program to be easily loaded and deployed. 
 */

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
    /*
     * The first part of the constructor loads the vertex and fragment
     * shaders from their source files. OpenGL compiles a shader from
     * a string containing the source code, so the contents of the files
     * have to be loaded and stored in string objects prior to compilation. 
     */

    // Load shader code from source files
    std::ifstream vShaderFile(vertexPath );
    std::stringstream vShaderStream;
    if (vShaderFile.good()) {
        vShaderStream << vShaderFile.rdbuf();
    }
    else {
        std::cout << "Error: Unable to open vertex Shader" << std::endl;
    }

    std::stringstream fShaderStream;
    std::ifstream fShaderFile(fragmentPath);
    if (fShaderFile.good()) {
        fShaderStream << fShaderFile.rdbuf();
    }
    else {
        std::cout << "Error: Unable to open fragment Shader" << std::endl;
    }
    
    // Compile the shaders
    unsigned int vertex = compileShader(GL_VERTEX_SHADER, vShaderStream.str());
    unsigned int fragment = compileShader(GL_FRAGMENT_SHADER, fShaderStream.str());

    /*
     * After the shaders are compiled, the graphics pipeline can be constructed
     * by creating a new OpenGL program and attaching the shaders. After the 
     * program has been formed there is no need to keep the individual shader
     * objects so they can be deleted. 
     */

    // Create program and link shaders while reporting any errors
    int success = 0;
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) { // Error occurs
        std::vector<char> infoLog(512);
        glGetProgramInfoLog(ID, 512, nullptr, &infoLog[0]);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED" << std::endl;
        for (auto c : infoLog) {
            std::cout << c;
        };
        std::cout << std::endl;
    }

    // Shader objects can be deleted after program construction
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader()
// Delete program from OpenGL when class instance is destroyed
{
    glDeleteProgram(ID);
}

void Shader::use()
// Convenient member function for loading the shader program
{
    glUseProgram(ID);
}

/*
 * The following functions are all used to set different types of 
 * uniform variables in the shader program by passing the variable
 * name and the desired value.
 */
void Shader::setBool(const std::string name, bool value) const {
    glUseProgram(ID);
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string name, int value) const {
    glUseProgram(ID);
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string name, float value) const {
    glUseProgram(ID);
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

unsigned int compileShader(const GLenum shaderType, const std::string& source)
/*
 * This function takes as input a shader type and a string containing the shader
 * source code, then compiles the shader and returns its ID. If any errors occur,
 * the error log from OpenGL is retrieved and printed.
 */
{
    int success = 0;
    unsigned int shaderID = glCreateShader(shaderType);
    const char* source_c_str = source.c_str();
    glShaderSource(shaderID, 1, &source_c_str, nullptr);
    glCompileShader(shaderID);
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success) { // Error occurs
        std::vector<char> infoLog(512);
        glGetShaderInfoLog(shaderID, 512, nullptr, &infoLog[0]);
        std::cout << "ERROR::SHADER::\n" << source << "\n::COMPILATION_FAILED\n";
        for (auto c : infoLog) {
            std::cout << c;
        };
        std::cout << std::endl;
    };
    return shaderID;
};