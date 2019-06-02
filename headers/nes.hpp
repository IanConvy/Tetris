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
    std::vector<int> filledRows;
    std::unique_ptr<Piece> currPiece;
    Grid grid;
    PieceGenerator pieceGen;

    NESTetris();
    void runFrame();
    void runActiveFrame();
    void runFrozenFrame();
    void runClearFrame();
    void writePiece();
    void clearPiece();
};

void resetBool(std::map<const std::string, bool>& flags);

#endif