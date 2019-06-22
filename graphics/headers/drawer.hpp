#define GLEW_STATIC
#ifndef DRAWER
#define DRAWER

#include "../../game/headers/grid.hpp"
#include "../../game/headers/pieces.hpp"
#include "stb_image.hpp"
#include "shader.hpp"
#include "text.hpp"
#include <vector>
#include <string>
#include <memory>

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
    std::unique_ptr<Piece>* nextPieceSource;
    Grid* gridSource;
    int* lineCountSource;
    int* scoreSource;
    int* levelSource;
    std::vector<int>* lineTypeCountSource;
    Shader brdShader;
    TextDrawer textDrawer;
    
    BoardDrawer(std::string location);
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
    void assignNextPiece(std::unique_ptr<Piece>& piecePtr);
    void assignGrid(Grid& grid);
    void assignLineCount(int& lineCount);
    void assignScore(int& score);
    void assignLevel(int& level);
    void assignlineTypeCount(std::vector<int>& typecounts);
};

void createTexture(unsigned int& texID, std::string filePath);

#endif 