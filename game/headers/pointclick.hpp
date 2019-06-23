#ifndef POINTCLICK

#define POINTCLICK
#include <map>
#include <string>
#include <vector>
#include <memory>

#include "pieces.hpp"
#include "grid.hpp"

struct PointClick
{
    int startLevel;
    int firstThreshold;
    float pixelHeight, pixelWidth, bottomLeftX, bottomLeftY;
    std::map<const std::string, bool> commands, flags;
    std::map<const std::string, int> constants, dynamic;
    std::vector<int> filledRows;
    std::vector<int> lineScore, lineTypeCount;
    std::unique_ptr<Piece> currPiece, nextPiece;
    std::map<const std::string, bool>* pressedPtr;
    std::vector<double>* mousePosPtr;
    Grid gameGrid, displayGrid;
    PieceGenerator pieceGen;

    PointClick(int startLevel, int x0, int x1, int y0, int y1);
    void setCommands();
    void assignPressed(std::map<const std::string, bool>& pressedSource);
    void assignMousePos(std::vector<double>& posSource);
    void runFrame();
    void writePiece();
    void highlightPiece(bool collision);
    void unHighlightPiece();
    void updatePiece();
    void setConstants();
    void setEntryDelay();
    void checkLevel();
    void resetGame();
    std::vector<int> getGridPosition(double xpos, double ypos);
};

#endif