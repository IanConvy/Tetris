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
    int startLevel;
    int firstThreshold;
    std::map<const std::string, bool> commands, flags;
    std::map<const std::string, int> constants, dynamic;
    std::vector<int> filledRows;
    std::vector<int> lineScore, lineTypeCount;
    std::unique_ptr<Piece> currPiece, nextPiece;
    Grid grid;
    PieceGenerator pieceGen;

    NESTetris(int startLevel);
    void runFrame();
    void runActiveFrame();
    void runFrozenFrame();
    void runClearFrame();
    void writePiece();
    void clearPiece();
    void updatePiece();
    void setConstants();
    void setEntryDelay();
    void checkLevel();
    void resetGame();
};

void resetBool(std::map<const std::string, bool>& flags);


#endif