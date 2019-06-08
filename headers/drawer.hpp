#define GLEW_STATIC
#ifndef BOARD_DRAWER
#define BOARD_DRAWER

#include "stb_image.hpp"
#include "shader_s.hpp"
#include "pieces.hpp"
#include "text.hpp"
#include <vector>
#include <utility>
#include <string>

struct BoardDrawer
{
    unsigned int sqrArray; 
    unsigned int brdTexture, fontTexture;
    unsigned int sqrBuffer, sqrIndexBuffer;
    float gridHeight, gridWidth;
    std::vector<float> brdVertices;
    std::vector<float> playFieldPos, previewPos;
    std::vector<unsigned int> blockTextures;
    std::vector<unsigned int> pieceTexMap;
    float blockWidthSpacing, blockHeightSpacing; // Requires playFieldPos for initialization 
    Shader brdShader;
    TextDrawer textDrawer;
    
    BoardDrawer();
    ~BoardDrawer();
    void drawBoard();
    void drawSquare(std::vector<float> vertices, unsigned int texture);
    void drawPieceBlocks(const std::vector<std::vector<int>> blocks);
    void drawPreview(const PieceData& data);
    void drawLineCount(std::string lineCount);
    void drawScore(std::string score);
};

void createTexture(unsigned int& texID, std::string filePath);

#endif 