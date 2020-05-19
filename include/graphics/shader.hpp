#ifndef SHADER
#define SHADER

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <string>

class Shader 
{
    public:
     
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
    ~Shader();
    void use();
    void setBool(const std::string name, bool value) const;
    void setInt(const std::string name, int value) const;
    void setFloat(const std::string name, float value) const;

    private:

    unsigned int ID;
};

unsigned int compileShader(const GLenum shaderType, const std::string& sourcePath);

#endif