#ifndef DRAWER
#define DRAWER

#include "game/grid.hpp"
#include "game/pieces.hpp"
#include "graphics/stb_image.hpp"
#include "graphics/shader.hpp"
#include "graphics/text.hpp"

#include <vector>
#include <string>
#include <memory>
#include <map>

class BoardDrawer
{
    public:

    BoardDrawer(std::string location);
    ~BoardDrawer();
    void drawFrame();
    void assignNextPiece(std::unique_ptr<Piece>& piecePtr);
    void assignGrid(Grid& grid);
    void assignLineCount(int& lineCount);
    void assignScore(int& score);
    void assignLevel(int& level);
    void assignlineTypeCount(std::vector<int>& typecounts);
    void assignMiscData(std::map<std::string, float>& data);

    private:

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
    std::map<std::string, float>* miscDataSource;
    Shader brdShader;
    TextDrawer textDrawer;
    
    void drawBoard();
    void drawSquare(const std::vector<float>& vertices, unsigned int texture);
    void drawPieceBlocks();
    void drawPreview();
    void drawLineCount();
    void drawLineTypeCount();
    void drawScore();
    void drawLevel();
    void drawMiscData();
};

void createTexture(unsigned int& texID, std::string filePath);

#endif 