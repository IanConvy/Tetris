#ifndef PLAYER
#define PLAYER
#include "../../game/headers/pieces.hpp"
#include "../../game/headers/grid.hpp"

#include <vector>
#include <memory>

struct Player
{
    PieceGenerator pieceGen;
    Grid grid;
    std::unique_ptr<Piece> currPiece, nextPiece;

    Player();
    void sweepPiece(std::vector<std::vector<int>>& holder);
    void placePiece(int centerCol, int orient);
};
#endif