#define GLEW_STATIC
#include "headers/board_drawer.hpp"

#include <vector>
#include <iostream>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "headers/stb_image.hpp"
#include "headers/shader_s.hpp"

BoardDrawer::BoardDrawer() : brdShader("shaders/v_shader.glsl", "shaders/f_shader.glsl")
{
    std::vector<int> brdVertices = {
        // Position             // Texture
        0, 0, 0,       0, 1,
        1035, 0, 0,       1, 1,
        1035, 899, 0,      1, 0,
        0, 899, 0,     0, 0};
    
    std::vector<unsigned int> brdIndices = {
        0, 1, 3,
        1, 2, 3};  

    brdShader.setInt("totalWidth", 1035);
    brdShader.setInt("totalHeight", 899);
    glGenVertexArrays(1, &brdArray);
    glBindVertexArray(brdArray);

    unsigned int brdBuffer = 0;
    glGenBuffers(1, &brdBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, brdBuffer);
    glBufferData(GL_ARRAY_BUFFER, brdVertices.size() * sizeof(int), &brdVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(int), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(int), (void*)(3 * sizeof(int)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    
    unsigned brdTexBuffer = 0;
    glGenBuffers(1, &brdTexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, brdTexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, brdIndices.size() * sizeof(unsigned int), &brdIndices[0], GL_STATIC_DRAW);
    
    glGenTextures(1, &brdTexture);
    glBindTexture(GL_TEXTURE_2D, brdTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    stbi_set_flip_vertically_on_load(true); 
    int width = 0, height = 0, nrChannels = 0;
    unsigned char *data = stbi_load("images/tetrisboard.png", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    else {
        std::cout << "Failed to load texture." << std::endl;
    }
    stbi_image_free(data);
}

void BoardDrawer::drawBoard() 
{
    glBindTexture(GL_TEXTURE_2D, brdTexture);
    brdShader.use();
    glBindVertexArray(brdArray);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
