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
#include <iostream>

Evaluator::Evaluator(int height, int width) :
evalGrid{height, width},
moves{},
pieceGen{{"lrPiece", "llPiece", "srPiece", "slPiece", "iPiece", "tPiece", "sqPiece"}}
{}

void Evaluator::generateMoves(Board& startingBoard, Piece piece)
{
    moves.clear();
    moves.reserve(34);
    std::vector<Board> newMoves;
    getMoves(startingBoard, piece, newMoves);
    for (auto move : newMoves) {
        std::map<std::string, float> score{{"total", evaluateMove(move, pieceGen)}};
        moves.emplace_back(move, score);
    }
    std::sort(moves.begin(), moves.end(),  
        [](std::pair<Board, std::map<std::string, float>> i, std::pair<Board, std::map<std::string, float>>j) 
            {return i.second["total"] > j.second["total"];});
}

float evaluateMove(Board& move, PieceGenerator& pieceGen) {
    // std::vector<std::vector<Board>> pieceMoves(7, std::vector<Board>{});
    // auto pieceList = pieceGen.pieceList;
    // for (int i = 0; i < pieceList.size(); ++i) {
    //     getMoves(move, *pieceGen.getPiece(pieceList[i]), pieceMoves[i]);
    // }
    // float minPieceScore = 100000;
    // for (auto moves : pieceMoves) {
    //     float bestScore = -10000;
    //     for (auto move : moves) {
    //         float score = 0;
    //         if (getHoles(move.grid) > 0) {
    //             score = burnEval(move)["total"];
    //         }
    //         else {
    //             score = positionEval(move)["total"];
    //         }
    //         bestScore = (score > bestScore) ? score : bestScore;
    //     }
    //     minPieceScore = (bestScore < minPieceScore) ? bestScore : minPieceScore;
    // }
    // return minPieceScore;
    float score = 0;
    if (getHoles(move.grid) > 0) {
        score = burnEval(move)["total"];
    }
    else {
        score = positionEval(move)["total"];
    }
    return score;
}

std::map<std::string, float> positionEval(Board& move)
{
    std::map<std::string, float> scores;
    int well = 9;
    auto minHeight = getMinHeight(move.grid, well);
    auto avgHeight = getAverageHeight(move.grid);
    scores["holes"] = -10 * getHoles(move.grid);
    scores["surface"] = -0.25 * getRoughness(move.grid, 2);
    scores["lines"] = -0.25 * move.lineCount;
    scores["tetris"] = 20 * move.lineTypeCount[3];
    scores["well"] = colClear(move.grid, well);
    scores["minheight"] = (minHeight <= 10) ? 1 * minHeight : 10;
    scores["avgheight"] = (avgHeight >= 10) ? -1 * avgHeight : 0;
    scores["valleys"] = -5 * countValleys(move.grid, well);
    float eval = 0;
    for (auto& evalScore : scores) {
        eval += evalScore.second;
    }
    scores["total"] = eval;
    return scores;
}

std::map<std::string, float> burnEval(Board& move)
{
    std::map<std::string, float> scores;
    int highestHole = getHighestHole(move.grid);
    int depth = getHoleDepth(move.grid, highestHole);
    scores["high"] = -50 * (highestHole + 1);
    scores["depth"] = -1 * depth;
    scores["filled"] = 1 * getNumFilled(move.grid, highestHole);
    scores["surface"] = -0.25 * getRoughness(move.grid, 2);
    scores["maxheight"] = -0.25 * getMaxHeight(move.grid); 
    scores["valleys"] = -5 * countValleys(move.grid, -1);
        float eval = 0;
    for (auto& evalScore : scores) {
        eval += evalScore.second;
    }
    scores["total"] = eval - 1000;
    return scores;
}

int getHighestHole(Grid& grid)
{
    int firstHoleRow = 0;
    for (int i = grid.grid.size() - grid.width - 1; i >= 0; --i) {
        if (grid.grid[i] == 0 && grid.grid[i + grid.width] != 0) {
            firstHoleRow = i / grid.width;
            break;
        }
    }
    return firstHoleRow;
}

int getHoleDepth(Grid& grid, int row)
{
    auto endItr =  grid.grid.begin() + (row + 1)*grid.width;
    int depth = 0;
    for (auto itr = grid.grid.begin() + row*grid.width; itr < endItr; ++itr) {
        if (*itr == 0 && *(itr + grid.width) != 0) {
            for (auto colItr = itr + grid.width; colItr < grid.grid.end(); colItr += grid.width) {
                if (*colItr != 0) {
                    ++depth;
                }
            }
        }
    }
    return depth;
}

int getNumFilled(Grid& grid, int row) {
    auto endItr =  grid.grid.begin() + (row + 1)*grid.width;
    int filled = 0;
    for (auto itr = grid.grid.begin() + row*grid.width; itr < endItr && itr < itr + grid.width; ++itr) {
        if (*itr != 0) {
            ++filled;
        }
    }
    return filled;
}

void getMoves(Board& startingBoard, Piece piece, std::vector<Board>& container) {
    for (int orient = 0; orient < piece.data.coordOffsets.size(); ++orient) {
        for (int centerCol = 0; centerCol < startingBoard.grid.width; ++centerCol) {
            piece.setPosition(startingBoard.grid.height - 1, centerCol, orient);
            if (!startingBoard.grid.collisionCheck(piece.coords)) {
                Board newMove = startingBoard;
                while (!startingBoard.grid.collisionCheck(piece.coords)) {
                    piece.translate(-1, 0);
                }
                piece.translate(1, 0);
                newMove.placePiece(piece);
                container.push_back(newMove);
            }
        }
    }
}

int surfaceEval(std::vector<int>& firstSurface, unsigned int depth, PieceGenerator& pieceGen)
{
    auto pieceList = pieceGen.pieceList;
    std::map<const std::string, std::vector<std::vector<int>>> surfaces, newSurfaces;
    surfaces[""] = {firstSurface};
    int eval = 0;
    for (; eval < depth; ++eval) {
        newSurfaces.clear();
        for (auto pieceSurfaces : surfaces) {
            for (auto pieceName : pieceList) {
                auto newName = pieceSurfaces.first + pieceName;
                newSurfaces[newName] = {};
                bool anySurface = false;
                for (auto surface : pieceSurfaces.second) {
                    getConnectedSurfaces(surface, *pieceGen.getPiece(pieceName), newSurfaces[newName]);
                    if (!newSurfaces[newName].empty()) {
                        anySurface = true;
                    }
                }
                if (!anySurface) {
                    return eval;
                }
            }
        }
        surfaces = newSurfaces;
    }
    return eval;
}

void getConnectedSurfaces(std::vector<int>& surface, Piece piece, std::vector<std::vector<int>>& container)
{
    for (int orient = 0; orient < piece.data.numOrients; ++orient) {
        auto bottom = piece.data.bottomSurf[orient];
        for (int startPos = 0; startPos < surface.size() - bottom.size(); ++startPos) {
            bool match = true;
            for (int offset = 0; offset < bottom.size(); ++offset) {
                if (surface[startPos + offset] != bottom[offset]) {
                    match = false;
                }
            }
            if (match) {
                std::vector<int> newSurface = surface;
                auto top = piece.data.topSurf[orient];
                if (startPos > 0) {
                    newSurface[startPos - 1] += piece.data.sideHeights[orient][0];
                }
                for (int offset = 0; offset < top.size(); ++offset) {
                    newSurface[startPos + offset] = top[offset];
                }
                if (startPos + top.size() < newSurface.size()) {
                    newSurface[startPos + top.size()] += piece.data.sideHeights[orient][1];
                }
                container.push_back(newSurface);
            }
        }
    }
}

std::vector<int> getSurface(Grid& grid)
{
    std::vector<int> surface;
    auto upperHeights = getUpperHeights(grid);
    surface.reserve(upperHeights.size() - 1);
    for (auto itr = upperHeights.begin(); itr != upperHeights.end() - 1; ++itr) {
        int delta = *(itr + 1) - *itr;
        surface.push_back(delta);
    }
    return surface;
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

int getRoughness(Grid& grid, int threshold)
{
    auto upperHeights = getUpperHeights(grid);
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

bool tetrisReady(Grid grid)
{
    auto upperHeights = getUpperHeights(grid);
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

bool colClear(Grid& grid, int col)
{
    bool clear = true;
    for (auto itr = grid.grid.begin() + col; itr < grid.grid.end(); itr += grid.width) {
        if (*itr != 0) {
            clear = false;
        }
    }
    return clear;
}

int countValleys(Grid& grid, int well)
{
    auto surface = getSurface(grid);
    int valleys = 0;
    if (well != 0 && surface[0] > 2) {
        ++valleys;
    }
    for (int i = 1; i < surface.size(); ++i) {
        if (i != well && surface[i] > 2 && surface[i - 1] < -2) {
            ++valleys;
        }
    }
    if (well != surface.size() && surface[surface.size() - 1] < -2) {
        ++valleys;
    }
    return valleys;
}

int getMinHeight(Grid& grid, int well)
{
    int minHeight = 100000;
    auto heights = getUpperHeights(grid);
    for (int i = 0; i < heights.size(); ++i) {
        if (i != well && heights[i] < minHeight) {
            minHeight = heights[i];
        }
    }
    return minHeight;
}

int getMaxHeight(Grid& grid)
{
    int maxHeight = 0;
    auto heights = getUpperHeights(grid);
    for (int i = 0; i < heights.size(); ++i) {
        if (heights[i] > maxHeight) {
            maxHeight = heights[i];
        }
    }
    return maxHeight;
}

float getAverageHeight(Grid& grid)
{
    auto heights = getUpperHeights(grid);
    float sum = 0.0;
    for (auto height : heights) {
        sum += height;
    }
    return sum / heights.size();
}
