#ifndef NES

#define NES
#include <map>
#include <string>
#include <vector>

#include "pieces.hpp"
#include "grid.hpp"

struct NESTetris
{
    std::map<const std::string, bool> commands;
    std::vector<std::vector<int>> droppedBlockCoords;
    Piece currPiece;
    Grid grid;

    NESTetris();
    void runFrame();
    const std::vector<std::vector<int>>& getBlockCoords();
};

void resetBool(std::map<const std::string, bool>& flags);

#endif