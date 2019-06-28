#ifndef POINTCLICK

#define POINTCLICK
#include <map>
#include <utility>
#include <string>
#include <vector>
#include <memory>

#include "pieces.hpp"
#include "grid.hpp"
#include "board.hpp"
#include "../../ai/headers/evaluate.hpp"

struct PointClick
{
    int startLevel;
    int firstThreshold;
    float pixelHeight, pixelWidth, bottomLeftX, bottomLeftY;
    std::map<const std::string, bool> commands, flags;
    std::map<const std::string, int> constants, dynamic;
    Board board, boardBackup;
    std::vector<int> lineScore;
    std::unique_ptr<Piece> currPiece, nextPiece;
    std::map<const std::string, bool>* pressedPtr;
    std::vector<Board> record;
    std::vector<std::string> pieceSeq;
    std::vector<double>* mousePosPtr;
    std::map<std::string, float> eval;
    Grid displayGrid;
    PieceGenerator pieceGen;
    Evaluator evaluator;

    PointClick(int startLevel, int x0, int x1, int y0, int y1);
    void setCommands();
    void assignPressed(std::map<const std::string, bool>& pressedSource);
    void assignMousePos(std::vector<double>& posSource);
    void runFrame();
    void runGameFrame();
    void runAIFrame();
    void highlightPiece(bool collision);
    void unHighlightPiece();
    void placePiece();
    void updatePiece();
    void updateLevel();
    void updateScore();
    void setConstants();
    void setEntryDelay();
    void resetGame();
    void readMove(int move);
    void truncateRecord(int moveInclusive);
    void displayEvalMoves(int move_index);
    std::vector<int> getGridPosition(double xpos, double ypos);
};

#endif