#ifndef EVAL
#define EVAL

#include "../../game/headers/grid.hpp"
#include "../../game/headers/pieces.hpp"
#include "../../game/headers/board.hpp"

#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <utility>
#include <cmath>

struct Evaluator
{
    Grid evalGrid;
    std::vector<std::pair<Board, std::map<std::string, float>>> moves;
    PieceGenerator pieceGen;

    Evaluator(int height, int width);
    void generateMoves(Board& startingBoard, Piece piece);
    std::map<std::string, float> evaluateMove(Board& move);
};

std::vector<int> getLowerHeights(Grid& grid);
std::vector<int> getUpperHeights(Grid& grid);
int getRoughness(std::vector<int>& upperHeights, int threshold);
int getHoles(Grid& grid);
bool tetrisReady(Grid grid, std::vector<int> upperHeights);

#endif