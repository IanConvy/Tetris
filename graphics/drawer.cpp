#define GLEW_STATIC
#include "headers/drawer.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <memory>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "headers/stb_image.hpp"
#include "headers/shader.hpp"
#include "headers/text.hpp"
#include "../game/headers/pieces.hpp"
#include "../game/headers/grid.hpp"

BoardDrawer::BoardDrawer(std::string location) : 
brdShader(
    (location + std::string("/shaders/v_shader.glsl")).c_str(), 
    (location + std::string("/shaders/f_shader.glsl")).c_str()),
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
blockTextures(5, 0),
pieceTexMap{0, 0, 1, 1, 0, 2, 2, 2, 3, 4},
blockWidthSpacing{(playFieldPos[2] - playFieldPos[0])/gridWidth},
blockHeightSpacing{(playFieldPos[5] - playFieldPos[1])/gridHeight},
nextPieceSource{nullptr},
gridSource{nullptr},
lineCountSource{nullptr},
scoreSource{nullptr},
levelSource{nullptr},
lineTypeCountSource{nullptr}
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
    
    createTexture(brdTexture, location + std::string("/images/tetrisboard.png"));
    createTexture(fontTexture, location + std::string("/images/fontbitmap.png"));
    createTexture(blockTextures[0], location + std::string("/images/yellowblock.png"));
    createTexture(blockTextures[1], location + std::string("/images/redblock.png"));
    createTexture(blockTextures[2], location + std::string("/images/whiteblock.png"));
    createTexture(blockTextures[3], location + std::string("/images/allowedblock.png"));
    createTexture(blockTextures[4], location + std::string("/images/disallowedblock.png"));
}

BoardDrawer::~BoardDrawer()
{
    glDeleteTextures(1, &brdTexture);
    glDeleteTextures(1, &fontTexture);
    glDeleteTextures(blockTextures.size(), &blockTextures[0]);
    glDeleteBuffers(1, &sqrBuffer);
    glDeleteBuffers(1, &sqrIndexBuffer);
    glDeleteVertexArrays(1, &sqrArray);
}

void BoardDrawer::drawFrame()
{
    drawBoard();
    drawPieceBlocks();
    drawPreview();
    drawLineCount();
    drawLineTypeCount();
    drawScore();
    drawLevel();
}

void BoardDrawer::drawBoard() 
{
    drawSquare(brdVertices, brdTexture);
}

void BoardDrawer::drawPreview()
{
    if (nextPieceSource != nullptr) {
        const PieceData& data  = (*nextPieceSource)->data;
        int texture = blockTextures[pieceTexMap[data.index]];
        int minWidth = data.coordOffsets[0][0][1]; 
        int maxWidth = data.coordOffsets[0][0][1]; 
        int minHeight = data.coordOffsets[0][0][0];  
        int maxHeight = data.coordOffsets[0][0][0]; 

        for (auto& rowCol : data.coordOffsets[0]) {
            if (rowCol[0] < minHeight) {
                minHeight = rowCol[0];
            }
            if (rowCol[0] > maxHeight) {
                maxHeight = rowCol[0];
            }
            if (rowCol[1] < minWidth) {
                minWidth = rowCol[1];
            }
            if (rowCol[1] > maxWidth) {
                maxWidth = rowCol[1];
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
            const float x0 = rowCol[1]*spacing + widthOffset;
            const float x1 = (rowCol[1] + 1)*spacing + widthOffset;
            const float y0 = -rowCol[0]*spacing + heightOffset;
            const float y1 = -(rowCol[0] + 1)*spacing + heightOffset;
            std::vector<float> vertices = {
            x0, y1, 0,      0, 1,
            x1, y1, 0,      1, 1,
            x0, y0, 0,      0, 0,
            x1, y0, 0,      1, 0};
            drawSquare(vertices, texture);
        }
    }
}

void BoardDrawer::drawPieceBlocks() {
    if (gridSource != nullptr) {
        for (auto& rowColIndex : gridSource->getFilledBlocks()) {
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
}

void BoardDrawer::drawLineCount()
{
    if (lineCountSource != nullptr) {
        std::string lineCountRaw = std::to_string(*lineCountSource);
        std::string lineCountStr = std::string("lines-") + 
            (lineCountRaw.size() < 3 ? std::string(3 - lineCountRaw.size(), '0') + lineCountRaw : lineCountRaw);
        auto textVertices = textDrawer.getTextVertices(lineCountStr, 408, 695, 64, 95);
        for (auto& charVertices : textVertices) {
            drawSquare(charVertices, fontTexture);
        }
    }
}

void BoardDrawer::drawLineTypeCount()
{
    if (lineTypeCountSource != nullptr) {
        int type = 0;
        std::vector<std::string> typeLabels{"single - ", "double - ", "triple - ", "tetris - "};
        for (const auto& typeCount : *lineTypeCountSource) {
            std::string countRaw = std::to_string(typeCount);
            std::string countStr = typeLabels[type] + 
                (countRaw.size() < 3 ? std::string(3 - countRaw.size(), '0') + countRaw : countRaw);
            auto textVertices = textDrawer.getTextVertices(countStr, 68, 333, 630 + 50*type, 630 + 50*type + 18);
            for (auto& charVertices : textVertices) {
                drawSquare(charVertices, fontTexture);
            }
            ++type;
        }
    }
}

void BoardDrawer::drawScore()
{
    if (scoreSource != nullptr) {
        std::string scoreRaw = std::to_string(*scoreSource);
        std::string scoreStr = scoreRaw.size() < 6 ? std::string(6 - scoreRaw.size(), '0') + scoreRaw : scoreRaw;
        auto textVertices = textDrawer.getTextVertices(scoreStr, 774, 980, 258, 286);
        for (auto& charVertices : textVertices) {
            drawSquare(charVertices, fontTexture);
        }
    }
}

void BoardDrawer::drawLevel()
{
    if (levelSource != nullptr) {
        std::string levelRaw = std::to_string(*levelSource);
        std::string levelStr = levelRaw.size() < 2 ? std::string(2 - levelRaw.size(), '0') + levelRaw : levelRaw;
        auto textVertices = textDrawer.getTextVertices(levelStr, 843, 902, 642, 671);
        for (auto& charVertices : textVertices) {
            drawSquare(charVertices, fontTexture);
        }
    }
}

void BoardDrawer::assignGrid(Grid& grid)
{
    gridSource = &grid;
}

void BoardDrawer::assignLevel(int& level)
{
    levelSource = &level;
}

void BoardDrawer::assignLineCount(int& lineCount)
{
    lineCountSource = &lineCount;
}

void BoardDrawer::assignlineTypeCount(std::vector<int>& typecounts)
{
    lineTypeCountSource = &typecounts;
}

void BoardDrawer::assignNextPiece(std::unique_ptr<Piece>& piecePtr)
{
    nextPieceSource = &piecePtr;
}

void BoardDrawer::assignScore(int& score)
{
    scoreSource = &score;
}

void BoardDrawer::drawSquare(const std::vector<float>& vertices, unsigned int texture)
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