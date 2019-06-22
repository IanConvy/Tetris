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

std::vector<int> Player::getBestMove()
{
    std::vector<std::vector<int>> moves;
    auto origGrid = grid.grid;
    auto origLineCount = lineCount;
    auto origLineCountType = lineTypeCount;
    float bestEval = -10000;
    int bestIndex = 0;
    int i = 0;
    for (int orient = 0; orient < currPiece->data.coordOffsets.size(); ++orient) {
        for (int col = 0; col < grid.width; ++col) {
            currPiece->setPosition(grid.height - 1, col, orient);
            if (!grid.collisionCheck(currPiece->coords)) {
                placePiece(col, orient);
                auto filledRows = grid.getFilledRows();
                lineCount += filledRows.size();
                if (!filledRows.empty()) {
                    grid.clearRows(filledRows);
                    ++lineTypeCount[filledRows.size() - 1];
                }
                moves.push_back({currPiece->centerRow, col, orient});
                float eval = evaluateGrid(grid.grid, false);
                if (eval > bestEval) {
                    bestEval = eval;
                    bestIndex = i;
                }
                grid.grid = origGrid;
                lineCount = origLineCount;
                lineTypeCount = origLineCountType;
                ++i;
            }
        }
    }
    if (!moves.empty()) {
        return moves[bestIndex];
    }
    else {
        return std::vector<int>{};
    }
}

float Player::evaluateGrid(std::vector<int>& moveGrid, bool print = false)
{
    auto lowerHeight = getLowerHeights(grid.height, grid.width, moveGrid);
    auto upperHeight = getUpperHeights(grid.height, grid.width, moveGrid);
    float holes = getHoles(lowerHeight, upperHeight, moveGrid);
    float roughness = getRoughness(upperHeight);

    float heightMin = 20;
    for (auto itr = lowerHeight.begin(); itr != lowerHeight.end() - 1; ++itr) {
        if (*itr < heightMin) {
            heightMin = *itr;
        }
    }
    float heightMax = 0;
    for (auto itr = upperHeight.begin(); itr != upperHeight.end(); ++itr) {
        if (*itr > heightMax) {
            heightMax = *itr;
        }
    }
    float minHeightScore = (heightMin < 5) ? heightMin : 5;
    float maxHeightScore = (heightMax > 10) ? heightMax - 10 : 0;
    float nonTetris = lineTypeCount[0] + lineTypeCount[1] + lineTypeCount[2];

    float holeCoeff = -5;
    float roughCoeff = -0.25;
    float minHeightCoeff = 1;
    float maxHeightCoeff = -1;
    float tetrisCoeff = 10;
    float lineCoeff = -1;

    float eval = 
        holeCoeff*holes + 
        roughCoeff*roughness + 
        minHeightCoeff*minHeightScore + 
        maxHeightCoeff*maxHeightScore +
        lineCoeff*nonTetris + 
        tetrisCoeff*lineTypeCount[3];

    if (print) {
        std::cout << "\nEvaluation: "
            << "\n   Hole Score: " << holeCoeff*holes
            << "\n   Roughness Score: " << roughCoeff*roughness
            << "\n   MinHeight Score: " << minHeightCoeff*minHeightScore
            << "\n   MaxHeight Score: " << maxHeightCoeff*maxHeightScore
            << "\n   Line Score: " << lineCoeff*nonTetris
            << "\n   Tetris Score: " << tetrisCoeff*lineTypeCount[3]
            << "\n   Score: " << eval - tetrisCoeff*lineTypeCount[3] - lineCoeff*nonTetris
            << std::endl; 
    }
    return eval;
}

bool Player::bestMove()
{
    bool topout = false;
    auto rowColOrient = getBestMove();
    if (!rowColOrient.empty()) {
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
    else {
        topout = true;
    }
    if (lineCount > 230) {
        topout = true;
    }
    return topout;
}


