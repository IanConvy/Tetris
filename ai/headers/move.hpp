#ifndef MOVE
#define MOVE

#include "../../game/headers/grid.hpp"
#include "../../game/headers/pieces.hpp"
#include "../../game/headers/board.hpp"
#include <cmath>

std::vector<Grid> getCollisionMaps(Grid& grid, Piece piece);
void addCollisionBlocks(Grid& collisionMap, std::vector<std::vector<int>>& fillCoords, std::vector<std::vector<int>>& coordOffsets);
std::vector<std::vector<int>> getPositions(Grid& collisionMap, int das, int gravity, int startCol);
void getMoves(Board& startingBoard, std::vector<Board>& container, Piece piece, int das, int gravity, int startCol);
std::vector<std::vector<int>> getPossible(std::vector<Grid>& collisionMaps);
std::vector<std::vector<int>> getCollisionHeights(std::vector<Grid>& collisionMaps);
std::vector<std::vector<bool>> getColAccess(std::vector<std::vector<int>>& collideHeights, std::vector<Grid>& collisionMaps, int das, int gravity, int startCol);

#endif