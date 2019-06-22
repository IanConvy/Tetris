#include "headers/player.hpp"

#include "../game/headers/pieces.hpp"
#include "../game/headers/grid.hpp"
#include "headers/tools.hpp"

#include <vector>
#include <memory>

Player::Player() : 
grid{20, 10},
pieceGen{{"lrPiece", "llPiece", "srPiece", "slPiece", "iPiece", "tPiece", "sqPiece"}},
currPiece{nullptr},
nextPiece{nullptr},
score{0},
lineCount{0},
lineTypeCount{0, 0, 0, 0}
{
    Piece firstPiece = Piece(allPieces.find("tPiece")->second);
    firstPiece.setPosition(0, 4, 2);
    for (auto rowCol : firstPiece.coords) {
        grid.fill(rowCol[0], rowCol[1], firstPiece.data.index);
    }
    currPiece = pieceGen.getRandomPiece();
    nextPiece = pieceGen.getRandomPiece();
}

void Player::newPiece()
{
    currPiece.swap(nextPiece);
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

void Player::sweepPiece(std::vector<std::vector<int>>& gridHolder, std::vector<std::vector<int>>& moveHolder)
{
    auto origGrid = grid.grid;
    for (int orient = 0; orient < currPiece->data.coordOffsets.size(); ++orient) {
        for (int col = 0; col < grid.width; ++col) {
            currPiece->setPosition(grid.height - 1, col, orient);
            if (!grid.collisionCheck(currPiece->coords)) {
                placePiece(col, orient);
                auto filledRows = grid.getFilledRows();
                if (!filledRows.empty()) {
                    grid.clearRows(filledRows);
                }
                gridHolder.push_back(grid.grid);
                moveHolder.push_back({currPiece->centerRow, col, orient});
                grid.grid = origGrid;
            }
        }
    }
}

int Player::evaluateMoves(std::vector<std::vector<int>>& moveGrids)
{
    int bestEval = -100000;
    int bestIndex = 0;
    int i = 0;
    for (auto moveGrid : moveGrids) {
        auto lowerHeight = getLowerHeights(grid.height, grid.width, moveGrid);
        auto upperHeight = getUpperHeights(grid.height, grid.width, moveGrid);
        auto holes = getHoles(lowerHeight, upperHeight, moveGrid);
        auto roughness = getRoughness(upperHeight);
        int eval = -(100*holes + roughness);
        if (eval > bestEval) {
            bestEval = eval;
            bestIndex = i;
        }
        ++i;
    }
    return bestIndex;
}

void Player::bestMove()
{
    std::vector<std::vector<int>> grids, moves;
    sweepPiece(grids, moves);
    auto bestMoveIndex = evaluateMoves(grids);
    auto rowColOrient = moves[bestMoveIndex];
    currPiece->setPosition(rowColOrient[0], rowColOrient[1], rowColOrient[2]);
    grid.fillSet(currPiece->coords, currPiece->data.index);
    auto filledRows = grid.getFilledRows();
    lineCount += filledRows.size();
    if (!filledRows.empty()) {
        grid.clearRows(filledRows);
        ++lineTypeCount[filledRows.size() - 1];
    }
    newPiece();
}


