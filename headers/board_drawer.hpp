#define GLEW_STATIC
#ifndef BOARD_DRAWER
#define BOARD_DRAWER

#include "shader_s.hpp"
#include "pieces.hpp"
#include <vector>
#include <utility>

struct BoardDrawer
{
    unsigned int sqrArray; 
    unsigned int brdTexture;
    unsigned int sqrBuffer, sqrIndexBuffer;
    float gridHeight, gridWidth;
    std::vector<float> brdVertices;
    std::vector<float> playFieldPos, previewPos;
    std::vector<unsigned int> blockTextures;
    std::vector<unsigned int> pieceTexMap;
    float blockWidthSpacing, blockHeightSpacing; // Requires playFieldPos for initialization 
    Shader brdShader;
    
    BoardDrawer();
    ~BoardDrawer();
    void drawBoard();
    void drawBlock(const float x0, const float x1, const float y0, const float y1, const unsigned index);
    void drawPieceBlocks(const std::vector<std::vector<int>> blocks);
    void drawPreview(const PieceData& data);
};

void createTexture(unsigned int& texID, std::string filePath);

#endif 