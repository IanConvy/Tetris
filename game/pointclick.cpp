#include "headers/pointclick.hpp"

#include "headers/pieces.hpp"
#include "headers/grid.hpp"

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <iostream>

PointClick::PointClick(int startLevel, int x0, int x1, int y0, int y1) :
startLevel{startLevel},
firstThreshold{0},
pixelHeight{0},
pixelWidth{0},
bottomLeftX{0},
bottomLeftY{0},
commands{},
dynamic{},
flags{
    {"mouseLeftHeld", false},
    {"aHeld", false},
    {"sHeld", false},
    {"escHeld", false}},
lineScore{0, 0, 0, 0},
lineTypeCount{0, 0, 0, 0},
currPiece{nullptr},
nextPiece{nullptr},
pressedPtr{nullptr},
mousePosPtr{nullptr},
filledRows{},
gameGrid{20, 10},
displayGrid{20, 10},
pieceGen{{"lrPiece", "llPiece", "srPiece", "slPiece", "iPiece", "tPiece", "sqPiece"}}
{
    pixelHeight = 610;
    pixelWidth = x1 - x0;
    bottomLeftX = x0;
    bottomLeftY = 759;
    if (startLevel <= 9) firstThreshold = 10*(startLevel + 1);
    else if (startLevel > 9 && startLevel <= 15) firstThreshold = 100;
    else firstThreshold = 10*(startLevel - 5);
    resetGame();
};

void PointClick::resetGame()
{
    commands["doCCW"] =  false;
    commands["doCW"] = false;
    commands["reset"] = false;
    commands["placePiece"] = false;

    dynamic["lineCount"] =  0;
    dynamic["score"] =  0;
    dynamic["level"] = startLevel;
    
    lineTypeCount = {0, 0, 0, 0};
    filledRows.clear();
    gameGrid.reset();
    displayGrid.reset();
    currPiece = pieceGen.getRandomPiece();
    nextPiece = pieceGen.getRandomPiece();
    setConstants();
}

void PointClick::runFrame()
{
    unHighlightPiece();
    setCommands();
    auto& xyPos = *mousePosPtr;
    auto mouseRowCol = getGridPosition(xyPos[0], xyPos[1]);
    currPiece->setPosition(mouseRowCol[0], mouseRowCol[1], currPiece->orient);

    // Place Piece:
    if (commands["placePiece"]) {
        if (!gameGrid.collisionCheck(currPiece->coords)) {
            writePiece();
            filledRows = gameGrid.getFilledRows();
            if (!filledRows.empty()) {
                dynamic["lineCount"] += filledRows.size();
                dynamic["score"] += lineScore[filledRows.size() - 1];
                ++lineTypeCount[filledRows.size() - 1];
                checkLevel();
                gameGrid.clearRows(filledRows, false);
            }
            displayGrid.grid = gameGrid.grid;
            updatePiece();
            currPiece->setPosition(mouseRowCol[0], mouseRowCol[1], 0);
        }
    }
     // CCW Rotations:
    if (commands["doCCW"]) {
        currPiece->rotate(-1);
    }
    // CW Rotations:
    if (commands["doCW"]) {
        currPiece->rotate(1);
    }
    highlightPiece();

    // Reset:
    if (commands["reset"]) {
        resetGame();
    }
    
    for (auto& keyValue : commands) {
        keyValue.second = false;
    }
}

void PointClick::writePiece()
{
    gameGrid.fillSet(currPiece->coords, currPiece->data.index);
}

void PointClick::highlightPiece()
{
    displayGrid.fillSet(currPiece->coords, currPiece->data.index);
}

void PointClick::unHighlightPiece()
{
    for (auto rowCol : currPiece->coords) {
        displayGrid.fill(rowCol[0], rowCol[1], gameGrid.get(rowCol[0], rowCol[1]));
    }
}

void PointClick::updatePiece()
{
    currPiece.swap(nextPiece);
    nextPiece = pieceGen.getRandomPiece();
    currPiece->setPosition(19, 5, 0);
}

void PointClick::checkLevel()
{
    if (dynamic["lineCount"] >= firstThreshold) {
        dynamic["level"] = startLevel + (dynamic["lineCount"] - firstThreshold)/10 + 1;
        setConstants();
    }
}

void PointClick::setConstants()
{
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

void PointClick::setCommands()
{   
    auto& pressed = *pressedPtr;
    // Left Mouse Button:
    if (pressed["mouseLeft"] && !flags["mouseLeftHeld"]) {
        commands["placePiece"] = true;
        flags["mouseLeftHeld"] = true;
    }
    if (!pressed["mouseLeft"]) {
        flags["mouseLeftHeld"] = false;
    } 
    // A key:
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
    // Escape key:
    if (pressed["esc"] && !flags["escHeld"]) {
        commands["reset"] = true;
        flags["escHeld"] = true;
    }
    if (!pressed["esc"]) {
        flags["escHeld"] = false;
    }
}

std::vector<int> PointClick::getGridPosition(double xpos, double ypos)
{
    int row = (bottomLeftY - ypos) / (pixelHeight/gameGrid.height);
    int col = (xpos - bottomLeftX) / (pixelWidth/gameGrid.width);
    return std::vector<int>{row, col};
}

void PointClick::assignPressed(std::map<const std::string, bool>& pressedSource)
{
    pressedPtr = &pressedSource;
}

void PointClick::assignMousePos(std::vector<double>& posSource)
{
    mousePosPtr = &posSource;
}
