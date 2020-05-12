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
commands{},
dynamic{},
flags{},
lineScore{0, 0, 0, 0},
lineTypeCount{0, 0, 0, 0},
currPiece{nullptr},
nextPiece{nullptr},
pressedPtr{nullptr},
filledRows{},
grid{20, 10},
pieceGen{{"lPiece", "jPiece", "sPiece", "zPiece", "iPiece", "tPiece", "sqPiece"}}
{
    if (startLevel <= 9) firstThreshold = 10*(startLevel + 1);
    else if (startLevel > 9 && startLevel <= 15) firstThreshold = 100;
    else firstThreshold = 10*(startLevel - 5);
    resetGame();
};

void NESTetris::resetGame()
{
    commands["doCCW"] =  false;
    commands["doCW"] = false;
    commands["doLeft"] = false;
    commands["doRight"] =  false;
    commands["softDrop"] =  false;
    commands["leftDAS"] = false;
    commands["rightDAS"] = false;
    commands["clearDAS"] = false;
    commands["reset"] = false;

    dynamic["dropFrames"] = 0;
    dynamic["gravity"] = 0;
    dynamic["dasFrames"] = 0;
    dynamic["frozenFrames"] = 0;
    dynamic["clearFrames"] = 0;
    dynamic["totalFrames"] = 0;
    dynamic["lineCount"] =  0;
    dynamic["score"] =  0;
    dynamic["level"] = startLevel;

    flags["frozen"] = false;
    flags["afterFirst"] = false;
    
    lineTypeCount = {0, 0, 0, 0};
    filledRows.clear();
    grid.clear();
    currPiece = pieceGen.getRandomPiece();
    nextPiece = pieceGen.getRandomPiece();
    currPiece->setPosition(19, 5, 0);
    setConstants();
    dynamic["gravity"] = constants["setGravity"];
}

void NESTetris::runFrame()
{
    setCommands();
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
    grid.fillSet(currPiece->coords, 0);
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

void NESTetris::setCommands()
{   // A key:
    auto& pressed = *pressedPtr;
    if (pressed["a"] && !flags["aHeld"] && !flags["sHeld"]) {
        commands["doCCW"] = true;
        flags["aHeld"] = true;
    }
    if (!pressed["a"]) {
        flags["aHeld"] = false;
    }
    // S key:
    if (pressed["s"] && !flags["sHeld"] && !flags["aHeld"]) {
        commands["doCW"] = true;
        flags["sHeld"] = true;
    }
    if (!pressed["s"]) {
        flags["sHeld"] = false;
    }
    // Left key:
    if (pressed["left"] && flags["leftHeld"]) { // This being first is important to avoid double move.
            commands["leftDAS"] = true;
        }
    if (pressed["left"] && !flags["leftHeld"] && !flags["rightHeld"]) {
        flags["leftHeld"] = true;
        commands["doLeft"] = true;
        commands["clearDAS"] = true;
    }    
    if (!pressed["left"]) {
        flags["leftHeld"] = false;
    }
    // Right key:
    if (pressed["right"] && flags["rightHeld"]) { // This being first is important to avoid double move.
        commands["rightDAS"] = true;
    }
    if (pressed["right"] && !flags["rightHeld"] && !flags["leftHeld"]) {
        flags["rightHeld"] = true;
        commands["doRight"] = true;
        commands["clearDAS"] = true;
    }    
    if (!pressed["right"]) {
        flags["rightHeld"] = false;
    }
    // Down key:
    if (pressed["down"]) {
        commands["softDrop"] = true;
    }
    else {
        commands["softDrop"] = false;
    }
    // Escape key:
    if (pressed["esc"]) {
        if (!flags["escHeld"]) {
            commands["reset"] = true;
            flags["escHeld"] = true;
        }
    }
    else {
        flags["escHeld"] = false;
    }
}

void NESTetris::assignInputs(std::map<const std::string, bool>& pressedSource)
{
    pressedPtr = &pressedSource;
}

void resetBool(std::map<const std::string, bool>& bools)
{
    for (auto& keyValue : bools) {
        keyValue.second = false;
    }
}
