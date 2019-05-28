#include <map>
#include <string>
#include <vector>

#include "pieces.hpp"

struct NESTetris
{
    std::map<const std::string, bool> commands;
    std::vector<int> droppedBlocksRows, droppedBlocksCols;
    Piece currPiece;

    NESTetris();
    void runFrame();
    const std::vector<int>& getBlocksRows();
    const std::vector<int>& getBlocksCols();
};

void resetBool(std::map<const std::string, bool>& flags);