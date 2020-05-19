#ifndef BOARD
#define BOARD

#include "game/grid.hpp"
#include "game/pieces.hpp"

#include <string>
#include <vector>

class Board
{
    public:

    int lineCount;
    std::vector<int> lineTypeCount;
    Grid grid;

    Board(int height, int width);
    void reset();
    void placePiece(const Piece& piece);
};

#endif