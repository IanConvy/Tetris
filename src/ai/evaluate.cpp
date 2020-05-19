#include "ai/evaluate.hpp"

#include "ai/move.hpp"
#include "game/grid.hpp"
#include "game/pieces.hpp"
#include "game/board.hpp"

#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <utility>
#include <cmath>
#include <iostream>

void printGrid(int height, int width, std::vector<int>& grid)
{
    for (int row = height - 1; row >= 0; --row) {
        auto itr = grid.begin() + width*row;
        for (int col = 0; col < width; ++col) {
            std::cout << *itr << " ";
            ++itr;
        } 
        std::cout << "\n";
    }
    std::cout << std::endl;
}

Evaluator::Evaluator(int height, int width, int das, int gravity, int startCol) :
well{width - 1},
das{das},
gravity{gravity},
startCol{startCol},
moves{},
pieceGen{{"lPiece", "jPiece", "sPiece", "zPiece", "iPiece", "tPiece", "sqPiece"}}
{}

// void Evaluator::placePiece(Piece& piece)
// {
//     board.placePiece(piece);
//     for (auto pieceName : pieceGen.pieceList) {
//         auto offsets = pieceGen.getPiece(pieceName)->data.coordOffsets;
//         auto maps = collisionMaps[pieceName];
//         for (int orient = 0; orient < offsets.size(); ++orient) {
//             addCollisionBlocks(maps[orient], piece.coords, offsets[orient]);
//         }
//     }
// }

void Evaluator::generateMoves(Board& startingBoard, Piece piece)
{
    moves.clear();
    moves.reserve(34);
    std::vector<Board> newMoves;
    well = 0; // findWell(startingBoard.grid, well); // BROKEN WHEN SWITCHING GRID
    // getMoves(startingBoard, newMoves, piece, das, gravity, startCol); // BROKEN WHEN SWITCHING GRID
    for (auto move : newMoves) {
        std::map<std::string, float> score{{"total", evaluateMove(move, pieceGen, well)}};
        moves.emplace_back(move, score);
    }
    std::sort(moves.begin(), moves.end(),  
        [](std::pair<Board, std::map<std::string, float>> i, std::pair<Board, std::map<std::string, float>>j) 
            {return i.second["total"] > j.second["total"];});
}

void Evaluator::setGravity(int newGravity)
{
    gravity = newGravity;
}

float evaluateMove(Board& move, PieceGenerator& pieceGen, int well) {
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
    if (true /*getHoles(move.grid) > 0 // BROKEN WHEN SWITCHING GRID */) {
        score = burnEval(move, well)["total"];
    }
    else {
        score = positionEval(move, well)["total"];
    }
    return score;
}

// void getMoves(Board& startingBoard, Piece piece, std::vector<Board>& container, int gravity, int startCol, int das) {
//     auto grid = startingBoard.grid;
//     for (int orient = 0; orient < piece.data.coordOffsets.size(); ++orient) {
//         for (int centerCol = 0; centerCol < grid.width; ++centerCol) {
//             piece.setPosition(grid.height - 1, startCol, orient);
//             bool collide = false;
//             int direction = (centerCol < startCol) ? -1 : 1;
//             int col = startCol;
//             if (col != centerCol) {
//                 col += direction;
//                 piece.translate(0, direction);
//                 collide = grid.collisionCheck(piece.coords);
//                 for (int frame = 0; !collide && col != centerCol; ++frame) {
//                     if (frame % (gravity + 1) == 0) {
//                         piece.translate(-1, 0);
//                         collide = grid.collisionCheck(piece.coords);
//                     }
//                     if (frame % (das + 1) == 0) {
//                         piece.translate(0, direction);
//                         col += direction;
//                         collide = grid.collisionCheck(piece.coords);
//                     }
//                 }
//             }
//             else {
//                 collide = grid.collisionCheck(piece.coords);
//             }
//             if (!collide) {
//                 Board newMove = startingBoard;
//                 while (!grid.collisionCheck(piece.coords)) {
//                     piece.translate(-1, 0);
//                 }
//                 piece.translate(1, 0);
//                 newMove.placePiece(piece);
//                 container.push_back(newMove);
//             }
//         }
//     }
// }

std::vector<std::vector<bool>> getValidColumns(Grid& grid, Piece piece, int gravity, int startCol, int das)
{
    std::vector<std::vector<bool>> validCols(piece.data.coordOffsets.size(), std::vector<bool>(grid.width, false));
    for (int orient = 0; orient < piece.data.coordOffsets.size(); ++orient) {
        for (int direction : {-1, 1}) {
            int col = startCol;
            piece.setPosition(grid.height - 1, startCol, orient);
            if (!grid.collisionCheck(piece.coords)) {
                validCols[orient][startCol] = true;
                for (int frame = 1; frame < 100000; ++frame) {
                    if (frame % (gravity + 1) == 0) {
                        piece.translate(-1, 0);
                        if (grid.collisionCheck(piece.coords)) {
                            break;
                        }
                    }
                    if (frame == 1 || (frame - 1) % (das + 1) == 0) {
                        piece.translate(0, direction);
                        col += direction;
                        if (!grid.collisionCheck(piece.coords)) {
                            validCols[orient][col] = true;
                        }
                        else {
                            break;
                        }
                    }
                }
            }
        }
    }
    return validCols;
}

std::map<std::string, float> positionEval(Board& move, int well)
{
    std::map<std::string, float> scores;
    auto minHeight = 0; // getMinHeight(move.grid, well); // BROKEN WHEN SWITCHING GRID
    auto avgHeight = 0; // getAverageHeight(move.grid); // BROKEN WHEN SWITCHING GRID
    scores["holes"] = 0; // -10 * getHoles(move.grid); // BROKEN WHEN SWITCHING GRID
    scores["surface"] = 0; // -0.25 * getRoughness(move.grid, 2); // BROKEN WHEN SWITCHING GRID
    scores["lines"] = 0; // -0.25 * move.lineCount; // BROKEN WHEN SWITCHING GRID
    scores["tetris"] = 0; // 20 * move.lineTypeCount[3]; // BROKEN WHEN SWITCHING GRID
    scores["well"] = 0; // colClear(move.grid, well); // BROKEN WHEN SWITCHING GRID
    scores["minheight"] = 0; // (minHeight <= 10) ? 1 * minHeight : 10; // BROKEN WHEN SWITCHING GRID
    scores["avgheight"] = 0; // (avgHeight >= 10) ? -1 * avgHeight : 0; // BROKEN WHEN SWITCHING GRID
    scores["valleys"] = 0; // -5 * countValleys(move.grid, well); // BROKEN WHEN SWITCHING GRID
    float eval = 0;
    for (auto& evalScore : scores) {
        eval += evalScore.second;
    }
    scores["total"] = eval;
    return scores;
}

std::map<std::string, float> burnEval(Board& move, int well)
{
    std::map<std::string, float> scores;
    int highestHole = 0; // getHighestHole(move.grid); // BROKEN WHEN SWITCHING GRID
    int depth = 0; //getHoleDepth(move.grid, highestHole); // BROKEN WHEN SWITCHING GRID
    scores["high"] = 0;  // -20 * (highestHole + 1); // BROKEN WHEN SWITCHING GRID
    scores["depth"] = 0; // -1 * depth; // BROKEN WHEN SWITCHING GRID
    scores["filled"] = 0; // 1 * getNumFilled(move.grid, highestHole); // BROKEN WHEN SWITCHING GRID
    scores["surface"] = 0; // -0.25 * getRoughness(move.grid, 2); // BROKEN WHEN SWITCHING GRID
    scores["maxheight"] = 0; // -0.5 * getMaxHeight(move.grid) * getMaxHeight(move.grid); // BROKEN WHEN SWITCHING GRID
    scores["valleys"] = 0; // -5 * countValleys(move.grid, well); // BROKEN WHEN SWITCHING GRID
        float eval = 0;
    for (auto& evalScore : scores) {
        eval += evalScore.second;
    }
    scores["total"] = eval - 1000;
    return scores;
}

// BROKEN WHEN SWITCHING GRID
// int findWell(Grid& grid, int oldWell)
// {
//     int well = oldWell;
//     auto heights = getUpperHeights(grid);
//     for (int col = heights.size() - 1; col >= 0; --col) {
//         if (heights[col] == 0) {
//             well = col;
//             break;
//         }
//     }
//     return well;
// }

// BROKEN WHEN SWITCHING GRID
// int getHighestHole(Grid& grid)
// {
//     int firstHoleRow = 0;
//     for (int i = grid.grid.size() - grid.width - 1; i >= 0; --i) {
//         if (grid.grid[i] == 0 && grid.grid[i + grid.width] != 0) {
//             firstHoleRow = i / grid.width;
//             break;
//         }
//     }
//     return firstHoleRow;
// }

// BROKEN WHEN SWITCHING GRID
// int getHoleDepth(Grid& grid, int row)
// {
//     auto endItr =  grid.grid.begin() + (row + 1)*grid.width;
//     int depth = 0;
//     for (auto itr = grid.grid.begin() + row*grid.width; itr < endItr; ++itr) {
//         if (*itr == 0 && *(itr + grid.width) != 0) {
//             for (auto colItr = itr + grid.width; colItr < grid.grid.end(); colItr += grid.width) {
//                 if (*colItr != 0) {
//                     ++depth;
//                 }
//             }
//         }
//     }
//     return depth;
// }

// BROKEN WHEN SWITCHING GRID
// int getNumFilled(Grid& grid, int row) {
//     auto endItr =  grid.grid.begin() + (row + 1)*grid.width;
//     int filled = 0;
//     for (auto itr = grid.grid.begin() + row*grid.width; itr < endItr && itr < itr + grid.width; ++itr) {
//         if (*itr != 0) {
//             ++filled;
//         }
//     }
//     return filled;
// }

// BROKEN WHEN SWITCHING GRID
// std::vector<int> getSurface(Grid& grid)
// {
//     std::vector<int> surface;
//     auto upperHeights = getUpperHeights(grid);
//     surface.reserve(upperHeights.size() - 1);
//     for (auto itr = upperHeights.begin(); itr != upperHeights.end() - 1; ++itr) {
//         int delta = *(itr + 1) - *itr;
//         surface.push_back(delta);
//     }
//     return surface;
// }

// BROKEN WHEN SWITCHING GRID
// std::vector<int> getLowerHeights(Grid& grid)
// {
//     std::vector<int> heights;
//     for (int col = 0; col < grid.width; ++col) {
//         int currHeight = 0;
//         for (auto itr = grid.grid.begin() + col; *itr != 0 && currHeight < grid.height; itr += grid.width) {
//             ++currHeight;
//         }
//         heights.push_back(currHeight);
//     }
//     return heights;
// }

// BROKEN WHEN SWITCHING GRID
// std::vector<int> getUpperHeights(Grid& grid)
// {
//     std::vector<int> heights;
//     for (int col = 0; col < grid.width; ++col) {
//         int currHeight = grid.height - 1;
//         for (auto itr = grid.grid.end() - grid.width  + col; *itr == 0 && currHeight >= 0; itr -= grid.width) {
//             --currHeight;
//         }
//         heights.push_back(currHeight + 1);
//     }
//     return heights;
// }

// BROKEN WHEN SWITCHING GRID
// int getRoughness(Grid& grid, int threshold)
// {
//     auto upperHeights = getUpperHeights(grid);
//     int roughness = 0;
//     for (auto itr = upperHeights.begin() + 1; itr != upperHeights.end(); ++itr) {
//         int delta = std::abs(*itr - *(itr - 1));
//         if (delta >= threshold) {
//             roughness += delta;
//         }
//     }
//     return roughness;
// }

// BROKEN WHEN SWITCHING GRID
// int getHoles(Grid& grid)
// {
//     int holes = 0;
//     for (auto itr = grid.grid.begin(); itr != grid.grid.end() - grid.width; ++itr) {
//         if (*itr == 0 && *(itr + grid.width) != 0) {
//             ++holes;
//         }
//     }
//     return holes;
// }

// BROKEN WHEN SWITCHING GRID
// bool tetrisReady(Grid grid)
// {
//     auto upperHeights = getUpperHeights(grid);
//     bool ready = false;
//     for (int col = 0; col < grid.width; ++col) {
//         int floor = upperHeights[col];
//         if ((floor + 3) < grid.height) {
//             std::vector<std::vector<int>> blocks = {
//                 {floor, col}, {floor + 1, col}, {floor + 2, col}, {floor + 3, col}};
//             grid.fillSet(blocks, 1);
//             auto filled = grid.getFilledRows();
//             if (filled.size() == 4) {
//                 return true;
//             }
//             grid.fillSet(blocks, 0);
//         }
//     }
//     return false;
// }

// BROKEN WHEN SWITCHING GRID
// bool colClear(Grid& grid, int col)
// {
//     bool clear = true;
//     for (auto itr = grid.grid.begin() + col; itr < grid.grid.end(); itr += grid.width) {
//         if (*itr != 0) {
//             clear = false;
//         }
//     }
//     return clear;
// }

// BROKEN WHEN SWITCHING GRID
// int countValleys(Grid& grid, int well)
// {
//     auto surface = getSurface(grid);
//     int valleys = 0;
//     if (well != 0 && surface[0] > 2) {
//         ++valleys;
//     }
//     for (int i = 1; i < surface.size(); ++i) {
//         if (i != well && surface[i] > 2 && surface[i - 1] < -2) {
//             ++valleys;
//         }
//     }
//     if (well != surface.size() && surface[surface.size() - 1] < -2) {
//         ++valleys;
//     }
//     return valleys;
// }

// BROKEN WHEN SWITCHING GRID
// int getMinHeight(Grid& grid, int well)
// {
//     int minHeight = 100000;
//     auto heights = getUpperHeights(grid);
//     for (int i = 0; i < heights.size(); ++i) {
//         if (i != well && heights[i] < minHeight) {
//             minHeight = heights[i];
//         }
//     }
//     return minHeight;
// }

// BROKEN WHEN SWITCHING GRID
// int getMaxHeight(Grid& grid)
// {
//     int maxHeight = 0;
//     auto heights = getUpperHeights(grid);
//     for (int i = 0; i < heights.size(); ++i) {
//         if (heights[i] > maxHeight) {
//             maxHeight = heights[i];
//         }
//     }
//     return maxHeight;
// }

// BROKEN WHEN SWITCHING GRID
// float getAverageHeight(Grid& grid)
// {
//     auto heights = getUpperHeights(grid);
//     float sum = 0.0;
//     for (auto height : heights) {
//         sum += height;
//     }
//     return sum / heights.size();
// }
