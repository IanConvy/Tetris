#ifndef POINTCLICK
#define POINTCLICK

#include "game/pieces.hpp"
#include "game/grid.hpp"
#include "game/board.hpp"
#include "game/inputs.hpp"
#include "ai/evaluate.hpp"

#include <map>
#include <string>
#include <vector>
#include <memory>

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