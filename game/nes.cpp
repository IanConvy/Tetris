#include "headers/nes.hpp"

#include "headers/pieces.hpp"
#include "headers/grid.hpp"

#include <map>
#include <string>
#include <vector>
#include <memory>

NESTetris::NESTetris(int startLevel) :
startLevel{startLevel},
firstThreshold{0},
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
dynamic{
    {"dropFrames", 0},
    {"gravity", 0},
    {"dasFrames", 0},
    {"frozenFrames", 0},
    {"clearFrames", 0},
    {"totalFrames", 0},
    {"lineCount", 0},
    {"score", 0},
    {"level", startLevel}},
flags{
    {"frozen", false},
    {"afterFirst", false}},
lineScore{760, 1900, 5700, 22800},
lineTypeCount{0, 0, 0, 0},
currPiece{nullptr},
nextPiece{nullptr},
filledRows{},
grid{20, 10},
pieceGen{{"lrPiece", "llPiece", "srPiece", "slPiece", "iPiece", "tPiece", "sqPiece"}}
{
    setConstants();
    currPiece = pieceGen.getRandomPiece();
    nextPiece = pieceGen.getRandomPiece();
    currPiece->setPosition(19, 5, 0);
    dynamic["gravity"] = constants["setGravity"];
    writePiece();
    if (startLevel <= 9) firstThreshold = 10*(startLevel + 1);
    else if (startLevel > 9 && startLevel <= 15) firstThreshold = 100;
    else firstThreshold = 10*(startLevel - 5);
};

void NESTetris::runFrame()
{
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
    if (!flags["afterFirst"] && dynamic["totalFrames"] >= constants["firstDelay"]) {
        flags["afterFirst"] = true;
    }
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
        flags["afterFirst"] = true;
    }
    else {
        dynamic["gravity"] = constants["setGravity"];
    }
    if (flags["afterFirst"] && dynamic["dropFrames"] >= dynamic["gravity"]) {
        dynamic["dropFrames"] = 0;
        currPiece->translate(-1, 0);
        if (grid.collisionCheck(currPiece->coords)) {
            currPiece->translate(1, 0);
            setEntryDelay();
            flags["newPiece"] = true;
            writePiece();
            filledRows = grid.getFilledRows();
            if (!filledRows.empty()) {
                dynamic["lineCount"] += filledRows.size();
                dynamic["score"] += lineScore[filledRows.size() - 1];
                ++lineTypeCount[filledRows.size() - 1];
                checkLevel();
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
    grid.fillSet(currPiece->coords, currPiece->data.index);
}

void NESTetris::clearPiece()
{
    grid.clearSet(currPiece->coords);
}

void NESTetris::updatePiece()
{
    currPiece.swap(nextPiece);
    nextPiece = pieceGen.getRandomPiece();
    currPiece->setPosition(19, 5, 0);
}

void NESTetris::checkLevel()
{
    if (dynamic["lineCount"] >= firstThreshold) {
        dynamic["level"] = startLevel + (dynamic["lineCount"] - firstThreshold)/10 + 1;
        setConstants();
    }
}

void NESTetris::setConstants()
{
    constants["dropGravity"] = 1;
    constants["dasLimit"] = 15;
    constants["dasFloor"] = 10;
    constants["firstDelay"] = 96;
    constants["width"] = 10;
    constants["height"] = 20;
    lineScore = {
        40*(dynamic["level"] + 1),
        100*(dynamic["level"] + 1),
        300*(dynamic["level"] + 1),
        1200*(dynamic["level"] + 1)};
    if (dynamic["level"] <= 8) constants["setGravity"] = 47 - 5*dynamic["level"];
    else if (dynamic["level"] == 9) constants["setGravity"] = 5;
    else if (dynamic["level"] > 9 && dynamic["level"] <= 18) constants["setGravity"] = 4 - (dynamic["level"]-10)/3;
    else if (dynamic["level"] > 18 && dynamic["level"] <= 28) constants["setGravity"] = 1;
    else if (dynamic["level"] > 28) constants["setGravity"] = 0;
}

void NESTetris::setEntryDelay()
{
    int row = currPiece->centerRow;
    if (row <= 1) constants["entryDelay"] = 9;
    else if (row > 1 && row < 18) constants["entryDelay"] = 11 + 2*((row - 2)/4);
    else constants["entryDelay"] = 17;
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
        {"score", 0},
        {"level", startLevel}};
    flags = {
        {"frozen", false},
        {"afterFirst", false}};
    lineTypeCount = {0, 0, 0, 0};
    filledRows.clear();
    grid.reset();
    currPiece = pieceGen.getRandomPiece();
    nextPiece = pieceGen.getRandomPiece();
    currPiece->setPosition(19, 5, 0);
    setConstants();
}

void resetBool(std::map<const std::string, bool>& bools)
{
    for (auto& keyValue : bools) {
        keyValue.second = false;
    }
}