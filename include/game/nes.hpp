#ifndef NES
#define NES

#include "game/pieces.hpp"
#include "game/grid.hpp"
#include "game/inputs.hpp"
#include "game/board.hpp"

#include <map>
#include <string>
#include <vector>
#include <memory>

struct NESTetris
{
    int startLevel;
    int firstThreshold;
    std::map<const std::string, bool> commands, flags;
    std::map<const std::string, int> constants, dynamic;
    std::vector<int> filledRows;
    std::vector<int> lineScore;
    std::vector<std::string> pieceSeq;
    std::unique_ptr<Piece> currPiece, nextPiece;
    InputHandler* inputPtr;
    Board board;
    Grid displayGrid;
    PieceGenerator pieceGen;

    NESTetris(int startLevel);
    void setConstants(int level);
    void setCommands();
    void runFrame();
    void runActiveFrame();
    void runFrozenFrame();
    void runClearFrame();
    void displayPiece();
    void clearPiece();
    void updatePiece();
    void updateScore();
    void setEntryDelay();
    void checkLevel();
    void resetGame();
    void assignInput(InputHandler& inputSource);
};

void resetBool(std::map<const std::string, bool>& flags);


#endif