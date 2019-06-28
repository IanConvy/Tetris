#ifndef RECORD
#define RECORD

#include "grid.hpp"
#include "pieces.hpp"

#include <string>
#include <vector>

struct Board
{
    int lineCount;
    std::vector<int> lineTypeCount;
    Grid grid;

    Board(int height, int width);
    void reset();
    void placePiece(Piece& piece);
};

#endif