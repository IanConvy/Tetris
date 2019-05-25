#define GLEW_STATIC
#ifndef SHADER_H
#define SHADER_H

#include <string>

#include "GL/glew.h"
#include "GLFW/glfw3.h"

struct Shader 
{
    unsigned int ID;
    
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
    ~Shader();
    void use();
    void setBool(const std::string name, bool value) const;
    void setInt(const std::string name, int value) const;
    void setFloat(const std::string name, float value) const;
};

unsigned int compileShader(const GLenum shaderType, const std::string& sourcePath);

#endif