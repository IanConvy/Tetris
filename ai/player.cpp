#include "headers/player.hpp"

#include "../game/headers/pieces.hpp"
#include "../game/headers/grid.hpp"

#include <vector>
#include <memory>

Player::Player() : 
grid{20, 10},
pieceGen{{"lrPiece", "llPiece", "srPiece", "slPiece", "iPiece", "tPiece", "sqPiece"}},
currPiece{nullptr},
nextPiece{nullptr}
{
    Piece firstPiece = Piece(allPieces.find("tPiece")->second);
    firstPiece.setPosition(0, 4, 2);
    for (auto rowCol : firstPiece.coords) {
        grid.fill(rowCol[0], rowCol[1], firstPiece.data.index);
    }
    currPiece = pieceGen.getRandomPiece();
    nextPiece = pieceGen.getRandomPiece();
}

void Player::placePiece(int centerCol, int orient)
{
    currPiece->setPosition(grid.height - 1, centerCol, orient);
    while (!grid.collisionCheck(currPiece->coords)) {
        currPiece->translate(-1, 0);
    }
    currPiece->translate(1, 0);
    for (auto colRow : currPiece->coords) {
        if (grid.inBounds(colRow[0], colRow[1])) {
            grid.fill(colRow[0], colRow[1], currPiece->data.index);
        }
    }
}

void Player::sweepPiece(std::vector<std::vector<int>>& holder)
{
    auto origGrid = grid.grid;
    for (unsigned int orient = 0; orient < currPiece->data.coordOffsets.size(); ++orient) {
        for (int col = 0; col < grid.width; ++col) {
            currPiece->setPosition(grid.height - 1, col, orient);
            if (!grid.collisionCheck(currPiece->coords)) {
                placePiece(col, orient);
                holder.push_back(grid.grid);
                grid.grid = origGrid;
            }
        }
    }
}


