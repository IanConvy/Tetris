#include "headers/nes.hpp"
#include "headers/pieces.hpp"
#include "headers/grid.hpp"

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
droppedBlockCoords(4, {0, 0, 0}),
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
        droppedBlockCoords[i][0] = currPiece.coords[i].first;
        droppedBlockCoords[i][1] = currPiece.coords[i].second;
        droppedBlockCoords[i][2] = currPiece.data.index;
    }
    resetBool(commands);
}

const std::vector<std::vector<int>>& NESTetris::getBlockCoords()
{
    return droppedBlockCoords;
}

void resetBool(std::map<const std::string, bool>& bools)
{
    for (auto& keyValue : bools) {
        keyValue.second = false;
    }
}