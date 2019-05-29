#ifndef NES

#define NES
#include <map>
#include <string>
#include <vector>
#include <utility>

#include "pieces.hpp"
#include "grid.hpp"

struct NESTetris
{
    std::map<const std::string, bool> commands;
    std::vector<std::pair<int, int>> droppedBlockCoords;
    Piece currPiece;
    Grid grid;

    NESTetris();
    void runFrame();
    const std::vector<std::pair<int, int>>& getBlockCoords();
};

void resetBool(std::map<const std::string, bool>& flags);

#endif