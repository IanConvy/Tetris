#include "headers/nes.hpp"
#include "headers/pieces.hpp"

#include <map>
#include <string>
#include <vector>

NESTetris::NESTetris() :
commands{
    {"doCCW", false},
    {"doCW", false},
    {"doLeft", false},
    {"doRight", false},
    {"softDrop", false},
    {"leftDAS", false},
    {"rightDAS", false},
    {"clearDAS", false}},
currPiece{lrPiece},
droppedBlocksRows(4, 0),
droppedBlocksCols(4, 0)
{
    currPiece.setPosition(5, 5, 0);
};

void NESTetris::runFrame()
{
    if (commands["doCCW"]) {
        currPiece.rotate(-1);
    }
    if (commands["doCW"]) {
        currPiece.rotate(1);
    }
    if (commands["doLeft"]) {
        currPiece.translate(0, -1);
    }
    if (commands["doRight"]) {
        currPiece.translate(0, 1);
    }
    for (int i = 0; i < 4; ++i) {
        droppedBlocksRows[i] = currPiece.rows[i]; 
        droppedBlocksCols[i] = currPiece.cols[i];
    }
    resetBool(commands);
}

const std::vector<int>& NESTetris::getBlocksRows()
{
    return droppedBlocksRows;
}

const std::vector<int>& NESTetris::getBlocksCols()
{
    return droppedBlocksCols;
}

void resetBool(std::map<const std::string, bool>& bools)
{
    for (auto& keyValue : bools) {
        keyValue.second = false;
    }
}