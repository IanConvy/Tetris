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
};

std::vector<int> getLowerHeights(Grid& grid);
std::vector<int> getUpperHeights(Grid& grid);
int getRoughness(Grid& grid, int threshold);
int getHoles(Grid& grid);
bool tetrisReady(Grid grid);
std::vector<int> getSurface(Grid& grid);
void getConnectedSurfaces(std::vector<int>& surface, Piece piece, std::vector<std::vector<int>>& container);
int surfaceEval(std::vector<int>& firstSurface, unsigned int depth, PieceGenerator& pieceGen);
bool colClear(Grid& grid, int col);
int countValleys(Grid& grid, int well);
int getMinHeight(Grid& grid, int well);
int getHighestHole(Grid& grid);
int getHoleDepth(Grid& grid, int row);
int getNumFilled(Grid& grid, int row);
void getMoves(Board& startingBoard, Piece piece, std::vector<Board>& container);
float evaluateMove(Board& move, PieceGenerator& pieceGen);
std::map<std::string, float> burnEval(Board& move);
std::map<std::string, float> positionEval(Board& move);
float getHoleScore(Grid& grid);
int getJagged(Grid& grid, int well);
float getAverageHeight(Grid& grid);
int getMaxHeight(Grid& grid);
bool pruneMove(Grid& grid);


#endif