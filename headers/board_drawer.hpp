#define GLEW_STATIC
#ifndef BOARD_DRAWER
#define BOARD_DRAWER

#include "shader_s.hpp"
#include <vector>
#include <utility>

struct BoardDrawer
{
    unsigned int sqrArray; 
    unsigned int brdTexture;
    unsigned int sqrBuffer, sqrIndexBuffer;
    float gridHeight, gridWidth;
    std::vector<int> brdVertices;
    std::vector<int> playFieldPos;
    std::vector<unsigned int> blockTextures;
    std::vector<unsigned int> pieceTexMap;
    float blockWidthSpacing, blockHeightSpacing; // Requires playFieldPos for initialization 
    Shader brdShader;
    
    BoardDrawer();
    ~BoardDrawer();
    void drawBoard();
    void drawBlock(const int row, const int col, const unsigned index);
    void drawBlocks(const std::vector<std::vector<int>> blocks);
};

void createTexture(unsigned int& texID, std::string filePath);

#endif 