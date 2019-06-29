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
    auto lowerHeight = getLowerHeights(move.grid);
    auto upperHeight = getUpperHeights(move.grid);

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
    scores["holes"] = -5 * getHoles(move.grid);
    scores["rough"] = -0.25 * getRoughness(upperHeight, 3);
    // scores["minheight"] = 1 * (heightMin < 5) ? heightMin : 5;
    scores["maxheight"] = -0.75 * ((heightMax > 10) ? heightMax - 10 : 0);
    scores["tetris"] = 15 * move.lineTypeCount[3];
    scores["lines"] = -1 * move.lineCount;
    scores["tetrisready"] = 3 * tetrisReady(move.grid, upperHeight); 
    float total = 0;
    for (auto keyValue : scores) {
        total += keyValue.second;
    }
    scores["total"] = total;
    return scores;
}

std::vector<int> getLowerHeights(Grid& grid)
{
    std::vector<int> heights;
    for (int col = 0; col < grid.width; ++col) {
        int currHeight = 0;
        for (auto itr = grid.grid.begin() + col; *itr != 0 && currHeight < grid.height; itr += grid.width) {
            ++currHeight;
        }
        heights.push_back(currHeight);
    }
    return heights;
}

std::vector<int> getUpperHeights(Grid& grid)
{
    std::vector<int> heights;
    for (int col = 0; col < grid.width; ++col) {
        int currHeight = grid.height - 1;
        for (auto itr = grid.grid.end() - grid.width  + col; *itr == 0 && currHeight >= 0; itr -= grid.width) {
            --currHeight;
        }
        heights.push_back(currHeight + 1);
    }
    return heights;
}

int getRoughness(std::vector<int>& upperHeights, int threshold)
{
    int roughness = 0;
    for (auto itr = upperHeights.begin() + 1; itr != upperHeights.end(); ++itr) {
        int delta = std::abs(*itr - *(itr - 1));
        if (delta >= threshold) {
            roughness += delta;
        }
    }
    return roughness;
}

int getHoles(Grid& grid)
{
    int holes = 0;
    for (auto itr = grid.grid.begin(); itr != grid.grid.end() - grid.width; ++itr) {
        if (*itr == 0 && *(itr + grid.width) != 0) {
            ++holes;
        }
    }
    return holes;
}

bool tetrisReady(Grid grid, std::vector<int> upperHeights)
{
    bool ready = false;
    for (int col = 0; col < grid.width; ++col) {
        int floor = upperHeights[col];
        if ((floor + 3) < grid.height) {
            std::vector<std::vector<int>> blocks = {
                {floor, col}, {floor + 1, col}, {floor + 2, col}, {floor + 3, col}};
            grid.fillSet(blocks, 1);
            auto filled = grid.getFilledRows();
            if (filled.size() == 4) {
                return true;
            }
            grid.clearSet(blocks);
        }
    }
    return false;
}


// int getHoles(std::vector<int>& lowerHeights, std::vector<int>& upperHeights, std::vector<int>& grid)
// {
//     int holes = 0;
//     int width = lowerHeights.size();
//     for (int col = 0; col < width; ++col) {
//         auto startItr = grid.begin() + width*lowerHeights[col] + col;
//         auto endItr = grid.begin() + width*upperHeights[col] + col;
//         for (; startItr != endItr; startItr += width) {
//             if (*startItr == 0) {
//                 ++holes;
//             }
//         }
//     }
//     return holes;
// }
