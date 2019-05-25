#define GLEW_STATIC
#include "headers/shader_s.hpp"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
    // Retrieve the source code from provided file paths
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
    }

    // Compile the shaders
    unsigned int vertex = compileShader(GL_VERTEX_SHADER, vertexCode);
    unsigned int fragment = compileShader(GL_FRAGMENT_SHADER, fragmentCode);

    // Create shader program
    int success = 0;
    std::vector<char> infoLog(512);
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, nullptr, &infoLog[0]);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n";
        for (auto c : infoLog) {
            std::cout << c;
        }
        std::cout << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use()
{
    glUseProgram(ID);
}

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
{
    unsigned int shaderID = glCreateShader(shaderType);
    const char* source_c_str = source.c_str();
    glShaderSource(shaderID, 1, &source_c_str, nullptr);
    glCompileShader(shaderID);
    int success = 0;
    std::vector<char> infoLog(512);
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shaderID, 512, nullptr, &infoLog[0]);
        std::cout << "ERROR::SHADER::\n" << source << "\n::COMPILATION_FAILED\n";
        for (auto c : infoLog) {
            std::cout << c;
        };
        std::cout << std::endl;
    };
    return shaderID;
};