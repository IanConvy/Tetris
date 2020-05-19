#ifndef PLAYER
#define PLAYER

#include "game/pieces.hpp"
#include "game/grid.hpp"
#include "game/board.hpp"
#include "ai/evaluate.hpp"
#include "ai/tools.hpp"

#include <vector>
#include <memory>

struct Player
{
    int startLevel;
    int level;
    int firstThreshold;
    int score;
    int gravity;
    std::vector<int> lineScore;
    std::map<std::string, float> eval;

    Evaluator evaluator;
    PieceGenerator pieceGen;
    std::unique_ptr<Piece> currPiece, nextPiece;
    Board board;

    Player(int startLevel);
    bool nextMove();
    void newPiece();
    void updateScore();
    void updateLevel();
    void setConstants();
    void reset();
};
#endif