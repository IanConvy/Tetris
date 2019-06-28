#include "headers/evaluate.hpp"

#include "../game/headers/grid.hpp"
#include "../game/headers/pieces.hpp"
#include "../game/headers/board.hpp"

#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <utility>
#include <cmath>

Evaluator::Evaluator(int height, int width) :
evalGrid{height, width},
moves{},
pieceGen{{"lrPiece", "llPiece", "srPiece", "slPiece", "iPiece", "tPiece", "sqPiece"}}
{}

void Evaluator::generateMoves(Board& startingBoard, Piece piece)
{
    moves.clear();
    moves.reserve(34);
    Board newMove{evalGrid.height, evalGrid.width};
    evalGrid = startingBoard.grid;
    for (int orient = 0; orient < piece.data.coordOffsets.size(); ++orient) {
        for (int centerCol = 0; centerCol < evalGrid.width; ++centerCol) {
            piece.setPosition(evalGrid.height - 1, centerCol, orient);
            if (!evalGrid.collisionCheck(piece.coords)) {
                newMove = startingBoard;
                while (!evalGrid.collisionCheck(piece.coords)) {
                    piece.translate(-1, 0);
                }
                piece.translate(1, 0);
                newMove.placePiece(piece);
                moves.emplace_back(newMove, evaluateMove(newMove));
                evalGrid.clearSet(piece.coords);
            }
        }
    }
    std::sort(moves.begin(), moves.end(),  
        [](std::pair<Board, std::map<std::string, float>> i, std::pair<Board, std::map<std::string, float>>j) 
            {return i.second["total"] > j.second["total"];});
}

std::map<std::string, float> Evaluator::evaluateMove(Board& move) {
    auto lowerHeight = getLowerHeights(evalGrid.height, evalGrid.width, move.grid.grid);
    auto upperHeight = getUpperHeights(evalGrid.height, evalGrid.width, move.grid.grid);

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
    std::map<std::string, float> scores;
    scores["holes"] = -5 * getHoles(lowerHeight, upperHeight, move.grid.grid);
    scores["rough"] = -0.25 * getRoughness(upperHeight);
    scores["minheight"] = 1 * (heightMin < 5) ? heightMin : 5;
    scores["maxheight"] = -1 * (heightMax > 10) ? heightMax - 10 : 0;
    scores["tetris"] = 10 * move.lineTypeCount[3];
    scores["lines"] = -1 * move.lineCount;
    float total = 0;
    for (auto keyValue : scores) {
        total += keyValue.second;
    }
    scores["total"] = total;
    return scores;
}

std::vector<int> getLowerHeights(int height, int width, std::vector<int>& grid)
{
    std::vector<int> heights;
    for (int col = 0; col < width; ++col) {
        int currHeight = 0;
        for (auto itr = grid.begin() + col; *itr != 0 && currHeight < height; itr += width) {
            ++currHeight;
        }
        heights.push_back(currHeight);
    }
    return heights;
}

std::vector<int> getUpperHeights(int height, int width, std::vector<int>& grid)
{
    std::vector<int> heights;
    for (int col = 0; col < width; ++col) {
        int currHeight = height - 1;
        for (auto itr = grid.end() - width  + col; *itr == 0 && currHeight >= 0; itr -= width) {
            --currHeight;
        }
        heights.push_back(currHeight + 1);
    }
    return heights;
}

int getRoughness(std::vector<int>& upperHeights)
{
    int roughness = 0;
    for (auto itr = upperHeights.begin() + 1; itr != upperHeights.end(); ++itr) {
        roughness += std::abs(*itr - *(itr - 1));
    }
    return roughness;
}

int getHoles(std::vector<int>& lowerHeights, std::vector<int>& upperHeights, std::vector<int>& grid)
{
    int holes = 0;
    int width = lowerHeights.size();
    for (int col = 0; col < width; ++col) {
        auto startItr = grid.begin() + width*lowerHeights[col] + col;
        auto endItr = grid.begin() + width*upperHeights[col] + col;
        for (; startItr != endItr; startItr += width) {
            if (*startItr == 0) {
                ++holes;
            }
        }
    }
    return holes;
}
