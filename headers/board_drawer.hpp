#define GLEW_STATIC
#ifndef BOARD_DRAWER
#define BOARD_DRAWER

#include "shader_s.hpp"
#include <vector>

struct BoardDrawer
{
    unsigned int sqrArray; 
    unsigned int brdTexture, blockTexture;
    unsigned int sqrBuffer, sqrIndexBuffer;
    float gridHeight, gridWidth;
    std::vector<int> brdVertices;
    std::vector<int> playFieldPos;
    float blockWidthSpacing, blockHeightSpacing; // Requires playFieldPos for initialization 
    Shader brdShader;
    
    BoardDrawer();
    ~BoardDrawer();
    void drawBoard();
    void drawBlock(const int row, const int col);
};

void createTexture(unsigned int& texID, std::string filePath);

#endif 