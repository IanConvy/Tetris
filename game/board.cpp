#include "headers/board.hpp"

#include "headers/grid.hpp"
#include "headers/pieces.hpp"

Board::Board(int height, int width) :
/*
 * The Board class represents a Tetris playfield, handling the
 * addition of pieces and any resulting line clear that arise. It
 * serves as a way of separating the basic behavior of a Tetris grid 
 * from the specific mechanics of gameplay, which varies between modes.
 */ 
lineCount{0}, // Number of lines that have been cleared
lineTypeCount{{0, 0, 0, 0}}, // Number of singles, doubles, triples, and Tetrises
grid{height, width}
{}

void Board::reset()
/*
 * This function resets the line counts and clears the grid to prepare 
 * for a new game.
 */
{
    lineCount = 0;
    lineTypeCount = {0, 0, 0, 0};
    grid.clear();
}

void Board::placePiece(const Piece& piece)
/*
 * This function places a piece on the grid based on the coordinates in
 * the passed Piece class instance.  
 */
{
    grid.fillSet(piece.coords, piece.data.index);
    auto filledRows = grid.getFilledRows();
    if (!filledRows.empty()) {
        lineCount += filledRows.size();
        ++lineTypeCount[filledRows.size() - 1];
        grid.clearRows(filledRows);
    }

}

