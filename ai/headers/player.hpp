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
    void sweepPiece(std::vector<std::vector<int>>& gridHolder, std::vector<std::vector<int>>& posHolder);
    void placePiece(int centerCol, int orient);
    bool bestMove();
    void newPiece();
    int evaluateMoves(std::vector<std::vector<int>>& moveGrids);
};
#endif