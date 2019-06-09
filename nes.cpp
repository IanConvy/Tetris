#include "headers/nes.hpp"
#include "headers/pieces.hpp"
#include "headers/grid.hpp"

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <iomanip>

NESTetris::NESTetris() :
commands{
    {"doCCW", false},
    {"doCW", false},
    {"doLeft", false},
    {"doRight", false},
    {"softDrop", false},
    {"leftDAS", false},
    {"rightDAS", false},
    {"clearDAS", false},
    {"reset", false}},
constants{
    {"setGravity", 2},
    {"dropGravity", 2},
    {"dasLimit", 15},
    {"dasFloor", 10},
    {"entryDelay", 18},
    {"firstDelay", 96}},
dynamic{
    {"dropFrames", 0},
    {"gravity", 0},
    {"dasFrames", 0},
    {"frozenFrames", 0},
    {"clearFrames", 0},
    {"totalFrames", 0},
    {"lineCount", 0},
    {"score", 0}},
flags{
    {"frozen", false}},
lineScore{760, 1900, 5700, 22800},
currPiece{nullptr},
nextPiece{nullptr},
filledRows{},
grid{20, 10},
pieceGen{{"lrPiece", "llPiece", "srPiece", "slPiece", "iPiece", "tPiece", "sqPiece"}}
{
    currPiece = pieceGen.getRandomPiece();
    nextPiece = pieceGen.getRandomPiece();
    currPiece->setPosition(19, 5, 0);
    dynamic["gravity"] = constants["setGravity"];
    writePiece();
};

void NESTetris::runFrame()
{
    // std::cout << std::setw(2) << std::setfill('0') << "\r" << dynamic["dasFrames"] << std::flush;
    if (commands["reset"]) {
        resetGame();
    }
    if (flags["frozen"] == true) {
        runFrozenFrame();
    }
    else {
        if (!filledRows.empty()) {
            runClearFrame();
        }
        else {
            runActiveFrame();
        }
    }
    resetBool(commands);
    ++ dynamic["totalFrames"];
}

void NESTetris::runFrozenFrame()
{
    ++ dynamic["frozenFrames"];
    if (dynamic["frozenFrames"] >= constants["entryDelay"]) {
        dynamic["frozenFrames"] = 0;
        flags["frozen"] = false;
        updatePiece();
        writePiece();
    }
}

void NESTetris::runClearFrame()
{
    ++ dynamic["clearFrames"];
    switch(dynamic["clearFrames"]) {
        case 7:
            for (int row : filledRows) {
                grid.fill(row, 5, 0);
                grid.fill(row, 5 - 1, 0);
            }
            break;
        case 12:
            for (int row : filledRows) {
                grid.fill(row, 5 + 1, 0);
                grid.fill(row, 5 - 2, 0);
            }
            break;
        case 17:
            for (int row : filledRows) {
                grid.fill(row, 5 + 2, 0);
                grid.fill(row, 5 - 3, 0);
            }
            break;
        case 22:
            for (int row : filledRows) {
                grid.fill(row, 5 + 3, 0);
                grid.fill(row, 5 - 4, 0);
            }
            break;
        case 26:
            for (int row : filledRows) {
                grid.fill(row, 5 + 4, 0);
                grid.fill(row, 5 - 5, 0);
            }
            break;
    }
    if (dynamic["clearFrames"] >= (17 + constants["entryDelay"])) {
        dynamic["clearFrames"] = 0;
        filledRows.clear();
        grid.swapGrids();
        updatePiece();
    }
}

void NESTetris::runActiveFrame()
{   
    clearPiece();
    // DAS Clear:
    if (commands["clearDAS"]) {
        dynamic["dasFrames"] = 0;
    }
    // CCW Rotations:
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
    // Gravity:
    if (commands["softDrop"]) {
        dynamic["gravity"] = constants["dropGravity"];
    }
    else {
        dynamic["gravity"] = constants["setGravity"];
    }
    if (dynamic["totalFrames"] >= constants["firstDelay"] && dynamic["dropFrames"] >= dynamic["gravity"]) {
        dynamic["dropFrames"] = 0;
        currPiece->translate(-1, 0);
        if (grid.collisionCheck(currPiece->coords)) {
            currPiece->translate(1, 0);
            flags["newPiece"] = true;
            writePiece();
            filledRows = grid.getFilledRows();
            if (!filledRows.empty()) {
                dynamic["lineCount"] += filledRows.size();
                dynamic["score"] += lineScore[filledRows.size() - 1];
                grid.clearRows(filledRows, true);
            }    
            else{
                flags["frozen"] = true;
            }
        }
        else {
            writePiece();
        }
    }    
    else {
        dynamic["dropFrames"] += 1;
        writePiece();
    }
}

void NESTetris::writePiece()
{
    for (auto& colRow : currPiece->coords) {
        if (grid.inBounds(colRow.first, colRow.second)) {
            grid.fill(colRow.first, colRow.second, currPiece->data.index);
        }
    }
}

void NESTetris::clearPiece()
{
    for (auto& colRow : currPiece->coords) {
        if (grid.inBounds(colRow.first, colRow.second)) {
            grid.fill(colRow.first, colRow.second, 0);
        }
    }
}

void NESTetris::updatePiece()
{
    currPiece.swap(nextPiece);
    nextPiece = pieceGen.getRandomPiece();
    currPiece->setPosition(19, 5, 0);
}

void NESTetris::resetGame()
{
    commands = {
        {"doCCW", false},
        {"doCW", false},
        {"doLeft", false},
        {"doRight", false},
        {"softDrop", false},
        {"leftDAS", false},
        {"rightDAS", false},
        {"clearDAS", false},
        {"reset", false}};
    dynamic = {
        {"dropFrames", 0},
        {"gravity", 0},
        {"dasFrames", 0},
        {"frozenFrames", 0},
        {"clearFrames", 0},
        {"totalFrames", 0},
        {"lineCount", 0},
        {"score", 0}};
    flags = {
        {"frozen", false}};
    filledRows.clear();
    grid.reset();
    currPiece = pieceGen.getRandomPiece();
    nextPiece = pieceGen.getRandomPiece();
    currPiece->setPosition(19, 5, 0);
    dynamic["gravity"] = constants["setGravity"];
}

void resetBool(std::map<const std::string, bool>& bools)
{
    for (auto& keyValue : bools) {
        keyValue.second = false;
    }
}