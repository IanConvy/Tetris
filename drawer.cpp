#define GLEW_STATIC
#include "headers/drawer.hpp"

#include <vector>
#include <iostream>
#include <cmath>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "headers/stb_image.hpp"
#include "headers/shader_s.hpp"
#include "headers/pieces.hpp"
#include "headers/text.hpp"

BoardDrawer::BoardDrawer() : 
brdShader("shaders/v_shader.glsl", "shaders/f_shader.glsl"),
textDrawer{},
brdVertices{
    //        Position         Texture
        0,      0,      0,      0, 1,
        1035,   0,      0,      1, 1,
        0,      899,    0,      0, 0,
        1035,   899,    0,      1, 0},
playFieldPos{
    390, 159,   710, 159,
    390, 805,   710, 805},
previewPos{
    776, 416,   902, 416,
    776, 550,   902, 550},
gridHeight{20},
gridWidth{10},   
blockTextures(3, 0),
pieceTexMap{0, 0, 1, 1, 0, 2, 2, 2},
blockWidthSpacing{(playFieldPos[2] - playFieldPos[0])/gridWidth},
blockHeightSpacing{(playFieldPos[5] - playFieldPos[1])/gridHeight}
{    
    brdShader.setFloat("totalWidth", 1035);
    brdShader.setFloat("totalHeight", 899);
    glGenVertexArrays(1, &sqrArray);
    glBindVertexArray(sqrArray);

    glGenBuffers(1, &sqrBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, sqrBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    
    std::vector<unsigned int> sqrIndices = {
        0, 1, 2,
        1, 2, 3}; 
    glGenBuffers(1, &sqrIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sqrIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sqrIndices.size() * sizeof(unsigned int), &sqrIndices[0], GL_STATIC_DRAW);
    
    createTexture(brdTexture, "images/tetrisboard.png");
    createTexture(fontTexture, "images/fontbitmap.png");
    createTexture(blockTextures[0], "images/yellowblock.png");
    createTexture(blockTextures[1], "images/redblock.png");
    createTexture(blockTextures[2], "images/whiteblock.png");
}

BoardDrawer::~BoardDrawer()
{
    glDeleteTextures(1, &brdTexture);
    glDeleteTextures(blockTextures.size(), &blockTextures[0]);
    glDeleteBuffers(1, &sqrBuffer);
    glDeleteBuffers(1, &sqrIndexBuffer);
    glDeleteVertexArrays(1, &sqrArray);
}

void BoardDrawer::drawBoard() 
{
    drawSquare(brdVertices, brdTexture);
}

void BoardDrawer::drawPreview(const PieceData& data)
{
    int texture = blockTextures[pieceTexMap[data.index]];
    int minWidth = data.coordOffsets[0][0].second; 
    int maxWidth = data.coordOffsets[0][0].second; 
    int minHeight = data.coordOffsets[0][0].first;  
    int maxHeight = data.coordOffsets[0][0].first; 

    for (auto& rowCol : data.coordOffsets[0]) {
        if (rowCol.first < minHeight) {
            minHeight = rowCol.first;
        }
        if (rowCol.first > maxHeight) {
            maxHeight = rowCol.first;
        }
        if (rowCol.second < minWidth) {
            minWidth = rowCol.second;
        }
        if (rowCol.second > maxWidth) {
            maxWidth = rowCol.second;
        }
    }
    float spacing = (previewPos[2] - previewPos[0]) / 4;
    float height = spacing * (maxHeight - minHeight + 1);
    float width = spacing * (maxWidth - minWidth + 1);
    float targetHeight = (previewPos[5] + previewPos[1] + height) / 2;
    float targetWidth = (previewPos[2] + previewPos[0] - width) / 2;
    float cornerHeight = -spacing*minHeight;
    float cornerWidth = spacing*minWidth;
    float heightOffset = targetHeight - cornerHeight;
    float widthOffset = targetWidth - cornerWidth;

    for (auto& rowCol : data.coordOffsets[0]) {
        const float x0 = rowCol.second*spacing + widthOffset;
        const float x1 = (rowCol.second + 1)*spacing + widthOffset;
        const float y0 = -rowCol.first*spacing + heightOffset;
        const float y1 = -(rowCol.first + 1)*spacing + heightOffset;
        std::vector<float> vertices = {
        x0, y1, 0,      0, 1,
        x1, y1, 0,      1, 1,
        x0, y0, 0,      0, 0,
        x1, y0, 0,      1, 0};
        drawSquare(vertices, texture);
    }
}

void BoardDrawer::drawPieceBlocks(const std::vector<std::vector<int>> blocks) {
    for (auto& rowColIndex : blocks) {
        int row = rowColIndex[0];
        int col = rowColIndex[1];
        int texture = blockTextures[pieceTexMap[rowColIndex[2]]];
        const float x0 = playFieldPos[0] + col*blockWidthSpacing;
        const float x1 = playFieldPos[0] + (col + 1)*blockWidthSpacing;
        const float y0 = playFieldPos[5] - row*blockHeightSpacing;
        const float y1 = playFieldPos[5] - (row + 1)*blockHeightSpacing;
        std::vector<float> vertices = {
        x0, y1, 0,      0, 1,
        x1, y1, 0,      1, 1,
        x0, y0, 0,      0, 0,
        x1, y0, 0,      1, 0};
        drawSquare(vertices, texture);
    }
}

void BoardDrawer::drawLineCount(std::string lineCount)
{
    auto textVertices = textDrawer.getTextVertices(lineCount, 408, 695, 64, 95);
    for (auto& charVertices : textVertices) {
        drawSquare(charVertices, fontTexture);
    }
}

void BoardDrawer::drawScore(std::string score)
{
    auto textVertices = textDrawer.getTextVertices(score, 774, 980, 258, 286);
    for (auto& charVertices : textVertices) {
        drawSquare(charVertices, fontTexture);
    }
}

void BoardDrawer::drawSquare(std::vector<float> vertices, unsigned int texture)
{
    glBindTexture(GL_TEXTURE_2D, texture);
    brdShader.use();
    glBindVertexArray(sqrArray);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
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