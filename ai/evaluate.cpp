#include "headers/evaluate.hpp"

#include "../game/headers/grid.hpp"
#include "../game/headers/pieces.hpp"

#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <utility>
#include <cmath>



Evaluator::Evaluator(int height, int width) :
evalGrid{height, width}
{}

std::vector<Move> Evaluator::getMoves(PieceData& data, std::vector<int>& startingGrid)
{
    std::vector<Move> moves;
    moves.reserve(34);
    Piece piece{data};   
    for (int orient = 0; orient < data.coordOffsets.size(); ++orient) {
        for (int centerCol = 0; centerCol < evalGrid.width; ++centerCol) {
            piece.setPosition(evalGrid.height - 1, centerCol, orient);
            if (!evalGrid.collisionCheck(piece.coords)) {
                auto move = Move{{0, 0, 0, 0}};
                evalGrid.grid = startingGrid;
                while (!evalGrid.collisionCheck(piece.coords)) {
                    piece.translate(-1, 0);
                }
                piece.translate(1, 0);
                evalGrid.fillSet(piece.coords, data.index);
                auto filledRows = evalGrid.getFilledRows();
                if (!filledRows.empty()) {
                    ++move.lineTypeCount[filledRows.size() - 1];
                    evalGrid.clearRows(filledRows);
                }
                move.grid = evalGrid.grid;
                moves.push_back(move);
                evalGrid.clearSet(piece.coords);
            }
        }
    }
    return moves;
}

std::map<std::string, float> Evaluator::evaluateMove(Move& move) {
    auto lowerHeight = getLowerHeights(evalGrid.height, evalGrid.width, move.grid);
    auto upperHeight = getUpperHeights(evalGrid.height, evalGrid.width, move.grid);

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
    scores["holes"] = -5 * getHoles(lowerHeight, upperHeight, move.grid);
    scores["rough"] = -0.25 * getRoughness(upperHeight);
    scores["minHeight"] = 1 * (heightMin < 5) ? heightMin : 5;
    scores["maxHeight"] = -1 * (heightMax > 10) ? heightMax - 10 : 0;
    scores["tetris"] = 10 * move.lineTypeCount[3];
    scores["lines"] = -1 * move.lineTypeCount[0] + move.lineTypeCount[1] + move.lineTypeCount[2];
    float total = 0;
    for (auto keyValue : scores) {
        total += keyValue.second;
    }
    scores["total"] = total;
    return scores;
}

std::vector<std::pair<int, float>> Evaluator::rankMoves(const std::vector<Move>& moves)
{
    std::vector<std::pair<int, float>> scoresMoves;
    scoresMoves.reserve(moves.size());
    int i = 0;
    for (auto move : moves) {
        auto score = evaluateMove(move);
        scoresMoves.emplace_back(i, score["total"]);
        ++i;
    }
    std::sort(scoresMoves.begin(), scoresMoves.end(),  
        [](std::pair<int, float> i, std::pair<int, float> j) {return i.second > j.second;});
    return scoresMoves; 
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
