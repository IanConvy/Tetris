#ifndef PLAYER
#define PLAYER
#include "../../game/headers/pieces.hpp"
#include "../../game/headers/grid.hpp"
#include "../../game/headers/board.hpp"
#include "evaluate.hpp"
#include "tools.hpp"

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