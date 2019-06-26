#ifndef EVAL
#define EVAL

#include "../../game/headers/grid.hpp"
#include "../../game/headers/pieces.hpp"

#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <utility>
#include <cmath>

struct Move
{
    std::vector<int> lineTypeCount;
    std::vector<int> grid;
    std::vector<std::string> pieceSeq;
};

struct Evaluator
{
    Grid evalGrid;

    Evaluator(int height, int width);
    std::vector<Move> getMoves(PieceData& data, std::vector<int>& startingGrid);
    std::map<std::string, float> evaluateMove(Move& move);
    std::vector<std::pair<int, float>> rankMoves(const std::vector<Move>& moves);
};

std::vector<int> getLowerHeights(int height, int width, std::vector<int>& grid);
std::vector<int> getUpperHeights(int height, int width, std::vector<int>& grid);
int getRoughness(std::vector<int>& upperHeights);
int getHoles(std::vector<int>& lowerHeights, std::vector<int>& upperHeights, std::vector<int>& grid);

#endif