#ifndef RECORD
#define RECORD

#include "grid.hpp"
#include "pieces.hpp"

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