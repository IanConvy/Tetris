#ifndef EVAL
#define EVAL

#include "move.hpp"
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
    int das, gravity, startCol;
    std::vector<std::pair<Board, std::map<std::string, float>>> moves;
    PieceGenerator pieceGen;

    Evaluator(int height, int width, int das, int gravity, int startCol);
    void generateMoves(Board& startingBoard, Piece piece);
    void setGravity(int newGravity);
};

float evaluateMove(Board& move, PieceGenerator& pieceGen, int well);
std::map<std::string, float> burnEval(Board& move, int well);
std::map<std::string, float> positionEval(Board& move, int well);
// void getMoves(Board& startingBoard, Piece piece, std::vector<Board>& container, int gravity, int startCol, int das);
std::vector<std::vector<bool>> getValidColumns(Grid& grid, Piece piece, int orient);

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