#include "ai/move.hpp"
#include "game/grid.hpp"
#include "game/board.hpp"
#include <cmath>
#include <iostream>

std::vector<Grid> getCollisionMaps(Grid& grid, Piece piece) 
{
    std::vector<Grid> collisionMaps;
    collisionMaps.reserve(4);
    for (int orient = 0; orient < piece.data.numOrients; ++orient) {
        collisionMaps.emplace_back(grid.height, grid.width);
        for (int row = -1; row < grid.height; ++row) {
            for (int col = 0; col < grid.width; ++col) {
                piece.setPosition(row, col, orient);
                if (grid.collisionCheck(piece.coords)) {
                    collisionMaps[orient].fill(row, col, 1);
                    if (row + 1 < grid.height && collisionMaps[orient].get(row + 1, col) == 0) {
                        collisionMaps[orient].fill(row + 1, col, 2);
                    }
                }
            }
        }
    }
    return collisionMaps;
}

void addCollisionBlocks(Grid& grid, std::vector<std::vector<int>>& fillCoords, std::vector<std::vector<int>>& coordOffsets)
{
    for (auto rowColFill : fillCoords) {
        for (auto rowColOffset : coordOffsets) {
            int targetRow = rowColFill[0] - rowColOffset[0];
            int targetCol = rowColFill[1] - rowColOffset[1];
            grid.fill(targetRow, targetCol, 1);
        }
    }
}

std::vector<std::vector<int>> getPossible(std::vector<Grid>& collisionMaps)
{
    std::vector<std::vector<int>> possible;
    possible.reserve(50);
    for (int orient = 0; orient < collisionMaps.size(); ++orient) {
        auto map = collisionMaps[orient];
        for (int row = 0; row < map.height; ++row) {
            for (int col = 0; col < map.width; ++col) {
                if (map.get(row, col) == 2) {
                    possible.push_back({row, col, orient});
                }
            }
        }
    }
    return possible;
} 

// BROKEN WHEN SWITCHING GRID
// std::vector<std::vector<int>> getPositions(std::vector<Grid>& collisionMaps, int das, int gravity, int startCol)
// {
//     std::vector<std::vector<int>> positions;
//     if (collisionMaps[0].get(collisionMaps[0].height - 1, startCol) != 1) {
//         auto possible = getPossible(collisionMaps);
//         positions.reserve(possible.size());
//         auto collideHeights = getCollisionHeights(collisionMaps);
//         auto colAccesses = getColAccess(collideHeights, collisionMaps, das, gravity, startCol);
//         for (auto rowColOrient : possible) {
//             int row = rowColOrient[0];
//             int col = rowColOrient[1];
//             int orient = rowColOrient[2];
//             if (colAccesses[orient][col]) {
//                 if (row == collideHeights[orient][col] + 1) {
//                     positions.push_back(rowColOrient);
//                 }
//                 else if (
//                     (col - 1 >= 0 && colAccesses[orient][col - 1] && row >= collideHeights[orient][col - 1]) ||
//                     (col + 1 < colAccesses[orient].size() && colAccesses[orient][col + 1] && row >= collideHeights[orient][col + 1])
//                     ) {
//                         positions.push_back(rowColOrient);
//                 }
//                 else {
//                     int cwOrient = (orient + 1 < colAccesses.size()) ? orient + 1 : 0;
//                     int ccwOrient = (orient - 1 >= 0) ? orient - 1 : colAccesses.size() - 1;
//                     if (
//                         (colAccesses[cwOrient][col] && row >= collideHeights[cwOrient][col]) ||
//                         (colAccesses[ccwOrient][col] && row >= collideHeights[ccwOrient][col])
//                         ) {
//                             positions.push_back(rowColOrient);
//                     }
//                 }
//             }
//         }
//     }
//     return positions;
// }

// BROKEN WHEN SWITCHING GRID
// void getMoves(Board& startingBoard, std::vector<Board>& container, Piece piece, int das, int gravity, int startCol)
// {
//     auto collisionMaps = getCollisionMaps(startingBoard.grid, piece);
//     auto positions = getPositions(collisionMaps, das, gravity, startCol);
//     for (auto rowColOrient : positions) {
//         Board newMove = startingBoard;
//         piece.setPosition(rowColOrient[0], rowColOrient[1], rowColOrient[2]);
//         newMove.placePiece(piece);
//         container.push_back(newMove);
//     }
// }

// BROKEN WHEN SWITCHING GRID
// std::vector<std::vector<int>> getCollisionHeights(std::vector<Grid>& collisionMaps)
// {
//     std::vector<std::vector<int>> allHeights;
//     for (auto map : collisionMaps) {
//         std::vector<int> heights;
//         int width = map.width;
//         for (int col = 0; col < width; ++col) {
//             int currHeight = map.height - 1;
//             for (auto itr = map.grid.end() - width + col; (*itr == 0 || *itr == 2) && currHeight >= 0; itr -= width) {
//                 --currHeight;
//             }
//             heights.push_back(currHeight);
//         }
//         allHeights.push_back(heights);
//     }
//     return allHeights;
// }

std::vector<std::vector<bool>> getColAccess(
    std::vector<std::vector<int>>& collideHeights, std::vector<Grid>& collisionMaps, int das, int gravity, int startCol)
{
    std::vector<std::vector<bool>> colAccesses;
    for (int orient = 0; orient < collideHeights.size(); ++orient) {
        auto heights = collideHeights[orient];
        auto map = collisionMaps[orient];
        std::vector<bool> accessCol(heights.size(), false);
        for (int direction : {-1, 1}) {
            for (int col = startCol; col >= 0 || col < heights.size(); col += direction) {
                bool dasCheck = (col == startCol) || (
                        ((map.height - heights[col] - 1)*(gravity + 1) > (std::abs(col - startCol) - 1)*(das + 1) + 1) &&
                        ((map.height - heights[col - direction] - 1)*(gravity + 1) > (std::abs(col - startCol) - 1)*(das + 1) + 1) 
                );
                accessCol[col] = dasCheck;
                if (!dasCheck) {
                    break;
                }
            }
        }
        colAccesses.push_back(accessCol);
    }
    return colAccesses;
}