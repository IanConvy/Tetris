#include "headers/nes.hpp"
#include "headers/pieces.hpp"
#include "headers/grid.hpp"

#include <map>
#include <string>
#include <vector>
#include <utility>

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
droppedBlockCoords(4, {0, 0}),
grid{20, 10}
{
    currPiece.setPosition(5, 5, 0);
};

void NESTetris::runFrame()
{
    if (commands["doCCW"]) {
        currPiece.rotate(-1);
        if (grid.collisionCheck(currPiece.coords)) {
            currPiece.rotate(1);
        }
    }
    if (commands["doCW"]) {
        currPiece.rotate(1);
        if (grid.collisionCheck(currPiece.coords)) {
            currPiece.rotate(-1);
        }
    }
    if (commands["doLeft"]) {
        currPiece.translate(0, -1);
        if (grid.collisionCheck(currPiece.coords)) {
            currPiece.translate(0, 1);
        }
    }
    if (commands["doRight"]) {
        currPiece.translate(0, 1);
        if (grid.collisionCheck(currPiece.coords)) {
            currPiece.translate(0, -1);
        }
    }
    for (int i = 0; i < 4; ++i) {
        droppedBlockCoords[i] = currPiece.coords[i]; 
    }
    resetBool(commands);
}

const std::vector<std::pair<int, int>>& NESTetris::getBlockCoords()
{
    return droppedBlockCoords;
}

void resetBool(std::map<const std::string, bool>& bools)
{
    for (auto& keyValue : bools) {
        keyValue.second = false;
    }
}