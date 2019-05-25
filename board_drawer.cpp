#define GLEW_STATIC
#include "headers/board_drawer.hpp"

#include <vector>
#include <iostream>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "headers/stb_image.hpp"
#include "headers/shader_s.hpp"

BoardDrawer::BoardDrawer() : 
brdShader("shaders/v_shader.glsl", "shaders/f_shader.glsl"),
brdVertices{
    //        Position         Texture
        0,      0,      0,      0, 1,
        1035,   0,      0,      1, 1,
        0,      899,    0,      0, 0,
        1035,   899,    0,      1, 0},
playFieldPos{
    390, 159,   710, 159,
    390, 805,   710, 805}, 
gridHeight{20},
gridWidth{10},   
blockWidthSpacing{(playFieldPos[2] - playFieldPos[0])/gridWidth},
blockHeightSpacing{(playFieldPos[5] - playFieldPos[1])/gridHeight}
{    
    brdShader.setInt("totalWidth", 1035);
    brdShader.setInt("totalHeight", 899);
    glGenVertexArrays(1, &sqrArray);
    glBindVertexArray(sqrArray);

    glGenBuffers(1, &sqrBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, sqrBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(int), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(int), (void*)(3 * sizeof(int)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    
    std::vector<unsigned int> sqrIndices = {
        0, 1, 2,
        1, 2, 3}; 
    glGenBuffers(1, &sqrIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sqrIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sqrIndices.size() * sizeof(unsigned int), &sqrIndices[0], GL_STATIC_DRAW);
    
    createTexture(brdTexture, "images/tetrisboard.png");
    createTexture(blockTexture, "images/redblock.png");

}

BoardDrawer::~BoardDrawer()
{
    glDeleteTextures(1, &brdTexture);
    glDeleteTextures(1, &blockTexture);
    glDeleteBuffers(1, &sqrBuffer);
    glDeleteBuffers(1, &sqrIndexBuffer);
    glDeleteVertexArrays(1, &sqrArray);
}

void BoardDrawer::drawBoard() 
{
    glBindTexture(GL_TEXTURE_2D, brdTexture);
    brdShader.use();
    glBindVertexArray(sqrArray);
    glBufferData(GL_ARRAY_BUFFER, brdVertices.size() * sizeof(int), &brdVertices[0], GL_STATIC_DRAW);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void BoardDrawer::drawBlock(const int row, const int col)
{
    const int x0 = playFieldPos[0] + col*blockWidthSpacing;
    const int x1 = playFieldPos[0] + (col + 1)*blockWidthSpacing;
    const int y0 = playFieldPos[5] - row*blockHeightSpacing;
    const int y1 = playFieldPos[5] - (row + 1)*blockHeightSpacing;

    std::vector<int> blockVertices = {
        x0, y1, 0,      0, 1,
        x1, y1, 0,      1, 1,
        x0, y0, 0,      0, 0,
        x1, y0, 0,      1, 0};
    glBindTexture(GL_TEXTURE_2D, blockTexture);
    brdShader.use();
    glBindVertexArray(sqrArray);
    glBufferData(GL_ARRAY_BUFFER, brdVertices.size() * sizeof(int), &blockVertices[0], GL_STATIC_DRAW);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void createTexture(unsigned int& texID, std::string filePath)
{
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    stbi_set_flip_vertically_on_load(true); 
    int width = 0, height = 0, nrChannels = 0;
    unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    else {
        std::cout << "Failed to load texture." << std::endl;
    }
    stbi_image_free(data);    
}