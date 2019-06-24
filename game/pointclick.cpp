#include "headers/pointclick.hpp"

#include "headers/pieces.hpp"
#include "headers/grid.hpp"
#include "headers/record.hpp"

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
    {"mouseRightHeld", false},
    {"aHeld", false},
    {"sHeld", false},
    {"zHeld", false},
    {"xHeld", false},
    {"escHeld", false}},
record{},
pieceSeq{},
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
    commands["onScreen"] = false;
    commands["recordBack"] = false;
    commands["recordForward"] = false;

    dynamic["lineCount"] =  0;
    dynamic["score"] =  0;
    dynamic["level"] = startLevel;
    dynamic["mouseRow"] = -1;
    dynamic["mouseCol"] = -1;
    dynamic["lastPlacedRow"] = -1;
    dynamic["lastPlacedCol"] = -1;
    dynamic["move"] = 0;
    
    lineTypeCount = {0, 0, 0, 0};
    filledRows.clear();
    gameGrid.reset();
    displayGrid.reset();
    currPiece = pieceGen.getRandomPiece();
    nextPiece = pieceGen.getRandomPiece();
    setConstants();
    record.clear();
    pieceSeq.clear();
    pieceSeq.push_back(currPiece->data.name);
    pieceSeq.push_back(nextPiece->data.name);
    recordMove();
}

void PointClick::runFrame()
{
    setCommands();
    unHighlightPiece();

    // Read Record:
    if (commands["recordBack"] && dynamic["move"] > 0) {
        readMove(dynamic["move"] - 1);
        commands["onScreen"] = false;
    }
    else if (commands["recordForward"] && dynamic["move"] < record.size() - 1) {
        readMove(dynamic["move"] + 1);
    }

    // Move Piece:
    if (commands["onScreen"]) {
        currPiece->setPosition(dynamic["mouseRow"], dynamic["mouseCol"], currPiece->orient);
    }

     // CCW Rotations:
    if (commands["doCCW"]) {
        currPiece->rotate(-1);
    }
    // CW Rotations:
    if (commands["doCW"]) {
        currPiece->rotate(1);
    }
    
    // Place Piece:
    if (commands["placePiece"] && commands["onScreen"]) {
        if (!gameGrid.collisionCheck(currPiece->coords)) {
            ++dynamic["move"];
            dynamic["lastPlacedRow"] = dynamic["mouseRow"];
            dynamic["lastPlacedCol"] = dynamic["mouseCol"];
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
            if (dynamic["move"] == pieceSeq.size() - 1) {
                pieceSeq.push_back(nextPiece->data.name);
            }
            if (dynamic["move"] != record.size() - 1) {
                truncateRecord(dynamic["move"]);
            }
            recordMove();
            currPiece->setPosition(dynamic["mouseRow"], dynamic["mouseCol"], 0);
        }
    }

    // Display Piece:
    if (commands["onScreen"] 
        && (dynamic["mouseRow"] != dynamic["lastPlacedRow"] || dynamic["mouseCol"] != dynamic["lastPlacedCol"])) {
        highlightPiece(gameGrid.collisionCheck(currPiece->coords));
        dynamic["lastPlacedRow"] = -1;
        dynamic["lastPlacedCol"] = -1;
    }

    // Reset:
    if (commands["reset"]) {
        resetGame();
    }
    // Clear commands:
    for (auto& keyValue : commands) {
        keyValue.second = false;
    }
}

void PointClick::writePiece()
{
    gameGrid.fillSet(currPiece->coords, currPiece->data.index);
}

void PointClick::highlightPiece(bool collision)
{
    if (!collision) {
        displayGrid.fillSet(currPiece->coords, 8);
    }
    else {
        displayGrid.fillSet(currPiece->coords, 9);
    }
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
    if (dynamic["move"] < pieceSeq.size() - 1) {
        nextPiece = pieceGen.getPiece(pieceSeq[dynamic["move"] + 1]);
    }
    else {
        nextPiece = pieceGen.getRandomPiece();
    }
    currPiece->setPosition(19, 5, 0);
}

void PointClick::recordMove()
{
    record.push_back(MoveRecord{
        currPiece->data.name,
        nextPiece->data.name,
        gameGrid.grid,
        dynamic["level"],
        dynamic["lineCount"],
        lineTypeCount});
}

void PointClick::readMove(int move)
{
    dynamic["move"] = move;
    auto moveRecord = record[move];
    currPiece = pieceGen.getPiece(moveRecord.pieceName);
    nextPiece = pieceGen.getPiece(moveRecord.nextPieceName);
    gameGrid.grid = moveRecord.gameGrid;
    displayGrid.grid = moveRecord.gameGrid;
    dynamic["level"] = moveRecord.level;
    dynamic["lineCount"] = moveRecord.lineCount;
    lineTypeCount = moveRecord.lineTypeCount;
}

void PointClick::truncateRecord(int moveInclusive)
{
    record.erase(record.begin() + moveInclusive, record.end()); 
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
    auto& xyPos = *mousePosPtr; 
    // Mouse Pos:
    auto mouseRowCol = getGridPosition(xyPos[0], xyPos[1]);
    dynamic["mouseRow"] = mouseRowCol[0];
    dynamic["mouseCol"] = mouseRowCol[1];
    if (mouseRowCol[0] >= 0 && mouseRowCol[0] < gameGrid.height && mouseRowCol[1] >= 0 && mouseRowCol[1] < gameGrid.width) {
        commands["onScreen"] = true;
    }
    // Left Mouse Button:
    if (pressed["mouseLeft"] && !flags["mouseLeftHeld"]) {
        commands["placePiece"] = true;
        flags["mouseLeftHeld"] = true;
    }
    if (!pressed["mouseLeft"]) {
        flags["mouseLeftHeld"] = false;
    } 
    // Right Mouse Button:
    if (pressed["mouseRight"] && !flags["mouseRightHeld"]) {
        commands["doCW"] = true;
        flags["mouseRightHeld"] = true;
    }
    if (!pressed["mouseRight"]) {
        flags["mouseRightHeld"] = false;
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
    // Z key:
    if (pressed["z"] && !flags["zHeld"]) {
        commands["recordBack"] = true;
        flags["zHeld"] = true;
    }
    if (!pressed["z"]) {
        flags["zHeld"] = false;
    }
    // X key:
    if (pressed["x"] && !flags["xHeld"]) {
        commands["recordForward"] = true;
        flags["xHeld"] = true;
    }
    if (!pressed["x"]) {
        flags["xHeld"] = false;
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
    int row = (bottomLeftY >= ypos) ? (bottomLeftY - ypos) / (pixelHeight/gameGrid.height) : -1;
    int col = (bottomLeftX <= xpos) ? (xpos - bottomLeftX) / (pixelWidth/gameGrid.width) : -1;
    if (row >= gameGrid.height) {
        row = -1;
    }
    if (col >= gameGrid.width) {
        col = -1;
    }
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
