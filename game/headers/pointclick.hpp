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
#include "inputs.hpp"
#include "../../ai/headers/evaluate.hpp"

struct PointClick
{
    int startLevel;
    int firstThreshold;
    int windowHeight, windowWidth;
    double playHeight, playWidth, bottomLeftX, bottomLeftY;
    std::map<const std::string, bool> commands, flags;
    std::map<const std::string, int> constants, dynamic;
    Board board, boardBackup;
    std::vector<int> lineScore;
    std::unique_ptr<Piece> currPiece, nextPiece;
    InputHandler* inputPtr;
    std::vector<Board> record;
    std::vector<std::string> pieceSeq;
    std::map<std::string, float> eval;
    Grid displayGrid;
    PieceGenerator pieceGen;
    Evaluator evaluator;

    PointClick(int startLevel);
    void setCommands();
    void checkMouse();
    void checkWindow();
    void runFrame();
    void runGameFrame();
    void runAIFrame();
    void highlightPiece(bool collision);
    void unHighlightPiece();
    void nextMove();
    void updatePiece();
    void updateLevel();
    void updateScore();
    void setConstants();
    void setEntryDelay();
    void resetGame();
    void readMove(int move);
    void truncateRecord(int moveInclusive);
    void displayEvalMoves(unsigned int move_index);
    std::vector<int> getGridPosition(double xpos, double ypos);
    void assignInput(InputHandler& inputSource);
};

#endif