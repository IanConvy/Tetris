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
    int well;
    Grid evalGrid;
    std::vector<std::pair<Board, std::map<std::string, float>>> moves;
    PieceGenerator pieceGen;

    Evaluator(int height, int width);
    void generateMoves(Board& startingBoard, Piece piece);
};

float evaluateMove(Board& move, PieceGenerator& pieceGen, int well);
std::map<std::string, float> burnEval(Board& move, int well);
std::map<std::string, float> positionEval(Board& move, int well);
void getMoves(Board& startingBoard, Piece piece, std::vector<Board>& container);

int findWell(Grid& grid, int oldWell);
std::vector<int> getLowerHeights(Grid& grid);
std::vector<int> getUpperHeights(Grid& grid);
int getRoughness(Grid& grid, int threshold);
int getHoles(Grid& grid);
bool tetrisReady(Grid grid);
std::vector<int> getSurface(Grid& grid);
bool colClear(Grid& grid, int col);
int getHighestHole(Grid& grid);
int getHoleDepth(Grid& grid, int row);
int getNumFilled(Grid& grid, int row);
int countValleys(Grid& grid, int well);
float getAverageHeight(Grid& grid);
int getMaxHeight(Grid& grid);
int getMinHeight(Grid& grid, int well);

#endif