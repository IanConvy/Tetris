#include "headers/nes.hpp"
#include "headers/pieces.hpp"
#include "headers/grid.hpp"

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <iostream>

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
constants{
    {"setGravity", 20},
    {"dropGravity", 2},
    {"dasLimit", 16},
    {"dasFloor", 10},
    {"entryDelay", 18}},
dynamic{
    {"dropFrames", 0},
    {"gravity", 0},
    {"dasFrames", 0},
    {"frozenFraes", 0}},
flags{
    {"frozen", false}},
currPiece{nullptr},
droppedBlockCoords(4, {0, 0, 0}),
grid{20, 10},
pieceGen{{"lrPiece", "llPiece", "srPiece", "slPiece", "iPiece", "tPiece", "sqPiece"}}
{
    currPiece = pieceGen.getRandomPiece();
    currPiece->setPosition(19, 5, 0);
    dynamic["gravity"] = constants["setGravity"];
};

void NESTetris::runFrame()
{
    if (flags["frozen"] == true) {
        runFrozenFrame();
    }
    else {
        runActiveFrame();
    }
    resetBool(commands);
}

void NESTetris::runFrozenFrame()
{
    dynamic["frozenFrames"] += 1;
    if (dynamic["frozenFrames"] >= constants["entryDelay"]) {
        dynamic["frozenFrames"] = 0;
        flags["frozen"] = false;
    }
}

void NESTetris::runActiveFrame()
{   // CCW Rotations:
    if (commands["doCCW"]) {
        currPiece->rotate(-1);
        if (grid.collisionCheck(currPiece->coords)) {
            currPiece->rotate(1);
        }
    }
    // CW Rotations:
    if (commands["doCW"]) {
        currPiece->rotate(1);
        if (grid.collisionCheck(currPiece->coords)) {
            currPiece->rotate(-1);
        }
    }
    // Left Translations:
    if (commands["doLeft"]) {
        currPiece->translate(0, -1);
        if (grid.collisionCheck(currPiece->coords)) {
            currPiece->translate(0, 1);
            dynamic["dasFrames"] = constants["dasLimit"];
        }
    }
    if (commands["leftDAS"]) {
        if (dynamic["dasFrames"] >= constants["dasLimit"]) {
            dynamic["dasFrames"] = constants["dasFloor"];
            currPiece->translate(0, -1);
            if (grid.collisionCheck(currPiece->coords)) {
                currPiece->translate(0, 1);
                dynamic["dasFrames"] = constants["dasLimit"];
            }
        }
        else {
            dynamic["dasFrames"] += 1;
        }
    }
    // Right Translations:
    if (commands["doRight"]) {
        currPiece->translate(0, 1);
        if (grid.collisionCheck(currPiece->coords)) {
            currPiece->translate(0, -1);
            dynamic["dasFrames"] = constants["dasLimit"];
        }
    }
    if (commands["rightDAS"]) {
        if (dynamic["dasFrames"] >= constants["dasLimit"]) {
            dynamic["dasFrames"] = constants["dasFloor"];
            currPiece->translate(0, 1);
            if (grid.collisionCheck(currPiece->coords)) {
                currPiece->translate(0, -1);
                dynamic["dasFrames"] = constants["dasLimit"];
            }
        }
        else {
            dynamic["dasFrames"] += 1;
        }
    }
    // DAS Clear:
    if (commands["clearDAS"]) {
        dynamic["dasFrames"] = 0;
    }
    // Gravity:
    if (commands["softDrop"]) {
        dynamic["gravity"] = constants["dropGravity"];
    }
    else {
        dynamic["gravity"] = constants["setGravity"];
    }
    if (dynamic["dropFrames"] >= dynamic["gravity"]) {
        dynamic["dropFrames"] = 0;
        currPiece->translate(-1, 0);
        if (grid.collisionCheck(currPiece->coords)) {
            currPiece->translate(1, 0);
            for (auto& rowCol : currPiece->coords) {
                grid.fill(rowCol.first, rowCol.second, currPiece->data.index);
            }
            currPiece = pieceGen.getRandomPiece();
            currPiece->setPosition(19, 5, 0);
            flags["frozen"] = true;
        }
    }    
    else {
        dynamic["dropFrames"] += 1;
    }
    for (int i = 0; i < 4; ++i) {
        droppedBlockCoords[i][0] = currPiece->coords[i].first;
        droppedBlockCoords[i][1] = currPiece->coords[i].second;
        droppedBlockCoords[i][2] = currPiece->data.index;
    }
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