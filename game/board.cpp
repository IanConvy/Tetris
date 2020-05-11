#include "headers/board.hpp"

#include "headers/grid.hpp"
#include "headers/pieces.hpp"

Board::Board(int height, int width) : 
lineCount{0},
lineTypeCount{{0, 0, 0, 0}},
grid{height, width}
{}

void Board::reset()
{
    lineCount = 0;
    lineTypeCount = {0, 0, 0, 0};
    grid.clear();
}

void Board::placePiece(Piece& piece)
{
    grid.fillSet(piece.coords, piece.data.index);
    auto filledRows = grid.getFilledRows();
    if (!filledRows.empty()) {
        lineCount += filledRows.size();
        ++lineTypeCount[filledRows.size() - 1];
        grid.clearRows(filledRows, false);
    }

}

