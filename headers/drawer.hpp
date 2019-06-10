#define GLEW_STATIC
#ifndef DRAWER
#define DRAWER

#include "stb_image.hpp"
#include "shader.hpp"
#include "text.hpp"
#include "nes.hpp"
#include <vector>
#include <string>

struct BoardDrawer
{
    unsigned int sqrArray; 
    unsigned int brdTexture, fontTexture;
    unsigned int sqrBuffer, sqrIndexBuffer;
    const float gridHeight, gridWidth;
    const std::vector<float> brdVertices;
    const std::vector<float> playFieldPos, previewPos;
    std::vector<unsigned int> blockTextures;
    const std::vector<unsigned int> pieceTexMap;
    const float blockWidthSpacing, blockHeightSpacing; // Requires playFieldPos for initialization 
    NESTetris& game;
    Shader brdShader;
    TextDrawer textDrawer;
    
    BoardDrawer(std::string location, NESTetris& game);
    ~BoardDrawer();
    void drawBoard();
    void drawSquare(const std::vector<float>& vertices, unsigned int texture);
    void drawPieceBlocks();
    void drawPreview();
    void drawLineCount();
    void drawLineTypeCount();
    void drawScore();
    void drawLevel();
    void drawFrame();
};

void createTexture(unsigned int& texID, std::string filePath);

#endif 