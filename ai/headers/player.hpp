#ifndef PLAYER
#define PLAYER
#include "../../game/headers/pieces.hpp"
#include "../../game/headers/grid.hpp"
#include "tools.hpp"

#include <vector>
#include <memory>

struct Player
{
    Grid grid;
    PieceGenerator pieceGen;
    std::unique_ptr<Piece> currPiece, nextPiece;
    int score;
    int lineCount;
    std::vector<int> lineTypeCount;

    Player();
    void placePiece(int centerCol, int orient);
    bool bestMove();
    void newPiece();
    std::vector<int> getBestMove();
    float evaluateGrid(std::vector<int>& grid, bool print);
};
#endif