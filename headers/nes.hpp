#ifndef NES

#define NES
#include <map>
#include <string>
#include <vector>
#include <memory>

#include "pieces.hpp"
#include "grid.hpp"

struct NESTetris
{
    std::map<const std::string, bool> commands, flags;
    std::map<const std::string, int> constants, dynamic;
    std::vector<std::vector<int>> droppedBlockCoords;
    std::unique_ptr<Piece> currPiece;
    Grid grid;
    PieceGenerator pieceGen;

    NESTetris();
    void runFrame();
    void runActiveFrame();
    void runFrozenFrame();
    const std::vector<std::vector<int>>& getBlockCoords();
};

void resetBool(std::map<const std::string, bool>& flags);

#endif