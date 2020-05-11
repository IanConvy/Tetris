#include "headers/pointclick.hpp"

#include "headers/pieces.hpp"
#include "headers/grid.hpp"
#include "headers/board.hpp"
#include "../ai/headers/evaluate.hpp"

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
    {"leftHeld", false},
    {"rightHeld", false},
    {"upHeld", false},
    {"downHeld", false},
    {"spaceHeld", false},
    {"escHeld", false},
    {"aiMode", false}},
record{},
pieceSeq{},
lineScore{0, 0, 0, 0},
currPiece{nullptr},
nextPiece{nullptr},
pressedPtr{nullptr},
mousePosPtr{nullptr},
eval{},
board{20, 10},
boardBackup{20, 10},
displayGrid{20, 10},
pieceGen{{"lrPiece", "llPiece", "srPiece", "slPiece", "iPiece", "tPiece", "sqPiece"}},
evaluator{20, 10, 5, 0, 5}
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
    commands["enterAIMode"] = false;
    commands["exitAIMode"] = false;
    commands["evalForward"] = false;
    commands["evalBackward"] = false;

    dynamic["score"] =  0;
    dynamic["level"] = startLevel;
    dynamic["mouseRow"] = -1;
    dynamic["mouseCol"] = -1;
    dynamic["lastPlacedRow"] = -1;
    dynamic["lastPlacedCol"] = -1;
    dynamic["move"] = 0;
    dynamic["evalIndex"] = 0;
    
    flags["aiMode"] = false;

    board.reset();
    displayGrid.clear();
    pieceSeq = pieceGen.getRandomSequence(750);
    updatePiece();
    setConstants();
    record.clear();
    record.push_back(board);
    eval = (true /*getHoles(board.grid) > 0 // BROKEN WHEN SWITCHING GRID*/ ) ? burnEval(board, evaluator.well) : positionEval(board, evaluator.well);
    evaluator.setGravity(constants["setGravity"]);
}

void PointClick::runFrame()
{
    setCommands();
    if (commands["enterAIMode"] && !flags["aiMode"]) {
        unHighlightPiece();
        evaluator.generateMoves(board, *currPiece);
        boardBackup = board;
        dynamic["evalIndex"] = 0;
        displayEvalMoves(dynamic["evalIndex"]);
        flags["aiMode"] = true;
    }
    if (commands["exitAIMode"] && flags["aiMode"]) {
        board = boardBackup;
        displayGrid = boardBackup.grid;
        eval = (true /*getHoles(board.grid) > 0 // BROKEN WHEN SWITCHING GRID*/ ) ? burnEval(board, evaluator.well) : positionEval(board, evaluator.well);
        flags["aiMode"] = false;
    }
    if (flags["aiMode"]) {
        runAIFrame();
    }
    else {
        runGameFrame();
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

void PointClick::runAIFrame()
{
    if (!evaluator.moves.empty()) {
        // See Eval Moves:
        if (commands["evalBackward"]) {
            dynamic["evalIndex"] = (dynamic["evalIndex"] > 0) ? dynamic["evalIndex"] - 1 : evaluator.moves.size() - 1;
            displayEvalMoves(dynamic["evalIndex"]);
        }
        else if (commands["evalForward"]) {
            dynamic["evalIndex"] = (dynamic["evalIndex"] < evaluator.moves.size() - 1 ) ? dynamic["evalIndex"] + 1 : 0;
            displayEvalMoves(dynamic["evalIndex"]);
        }
        // Place Piece:
        if (commands["placeAIPiece"]) {
            board = evaluator.moves[dynamic["evalIndex"]].first;
            boardBackup = board;
            nextMove();
            evaluator.generateMoves(board, *currPiece);
            dynamic["evalIndex"] = 0;
            displayEvalMoves(dynamic["evalIndex"]);
        }
    }
}

void PointClick::runGameFrame()
{
    unHighlightPiece();
    // Read Record:
    if (commands["recordBack"] && dynamic["move"] > 0) {
        readMove(dynamic["move"] - 1);
        commands["onScreen"] = false;
    }
    else if (commands["recordForward"] && dynamic["move"] < record.size() - 1) {
        readMove(dynamic["move"] + 1);
        commands["onScreen"] = false;
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
        if (!board.grid.collisionCheck(currPiece->coords)) {
            dynamic["lastPlacedRow"] = dynamic["mouseRow"];
            dynamic["lastPlacedCol"] = dynamic["mouseCol"];
            board.placePiece(*currPiece);
            nextMove();
            currPiece->setPosition(dynamic["mouseRow"], dynamic["mouseCol"], 0);
        }
    }
    // Display Piece:
    if (commands["onScreen"] 
        && (dynamic["mouseRow"] != dynamic["lastPlacedRow"] || dynamic["mouseCol"] != dynamic["lastPlacedCol"])) {
        highlightPiece(board.grid.collisionCheck(currPiece->coords));
        dynamic["lastPlacedRow"] = -1;
        dynamic["lastPlacedCol"] = -1;
    }
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
        displayGrid.fill(rowCol[0], rowCol[1], board.grid.get(rowCol[0], rowCol[1]));
    }
}

void PointClick::nextMove()
{
    if (dynamic["move"] != record.size() - 1) {
        truncateRecord(dynamic["move"]  + 1);
    }
    record.push_back(board);
    ++dynamic["move"];
    updateScore();
    updateLevel();
    updatePiece();
    displayGrid = board.grid;
    eval = (true /*getHoles(board.grid) > 0 // BROKEN WHEN SWITCHING GRID*/ ) ? burnEval(board, evaluator.well) : positionEval(board, evaluator.well);
}

void PointClick::readMove(int move)
{
    dynamic["move"] = move;
    board = record[move];
    displayGrid = board.grid;
    updatePiece();
    updateScore();
    updateLevel();
    eval = (true /*getHoles(board.grid) > 0 // BROKEN WHEN SWITCHING GRID*/ ) ? burnEval(board, evaluator.well) : positionEval(board, evaluator.well);
}

void PointClick::truncateRecord(int moveInclusive)
{
    record.erase(record.begin() + moveInclusive, record.end()); 
}

void PointClick::displayEvalMoves(unsigned int move_index)
{
    if (move_index < evaluator.moves.size()) {
        auto move = evaluator.moves[move_index];
        displayGrid = move.first.grid;
        board = move.first;
        eval = (true /*getHoles(board.grid) > 0 // BROKEN WHEN SWITCHING GRID*/ ) ? burnEval(board, evaluator.well) : positionEval(board, evaluator.well);
    }
}

void PointClick::updatePiece()
{
    currPiece = pieceGen.getPiece(pieceSeq[dynamic["move"]]);
    nextPiece = pieceGen.getPiece(pieceSeq[dynamic["move"] + 1]);
    currPiece->setPosition(19, 5, 0);
}

void PointClick::updateScore()
{
    dynamic["score"] = 
        lineScore[0] * board.lineTypeCount[0] +
        lineScore[1] * board.lineTypeCount[1] +
        lineScore[2] * board.lineTypeCount[2] +
        lineScore[3] * board.lineTypeCount[3];
}

void PointClick::updateLevel()
{
    if (board.lineCount >= firstThreshold) {
        dynamic["level"] = startLevel + (board.lineCount - firstThreshold)/10 + 1;
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
    if (mouseRowCol[0] >= 0 && mouseRowCol[0] < displayGrid.height && mouseRowCol[1] >= 0 && mouseRowCol[1] < displayGrid.width) {
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
    // Left key:
    if (pressed["left"] && !flags["leftHeld"]) {
        commands["evalBackward"] = true;
        flags["leftHeld"] = true;
    }
    if (!pressed["left"]) {
        flags["leftHeld"] = false;
    }
    // Right key:
    if (pressed["right"] && !flags["rightHeld"]) {
        commands["evalForward"] = true;
        flags["rightHeld"] = true;
    }
    if (!pressed["right"]) {
        flags["rightHeld"] = false;
    }
    // Up key:
    if (pressed["up"] && !flags["upHeld"]) {
        commands["enterAIMode"] = true;
        flags["upHeld"] = true;
    }
    if (!pressed["up"]) {
        flags["upHeld"] = false;
    }
    // Down key:
    if (pressed["down"] && !flags["downHeld"]) {
        commands["exitAIMode"] = true;
        flags["downHeld"] = true;
    }
    if (!pressed["down"]) {
        flags["downHeld"] = false;
    }
    // Space bar:
    if (pressed["space"] && !flags["spaceHeld"]) {
        commands["placeAIPiece"] = true;
        flags["spaceHeld"] = true;
    }
    if (!pressed["space"]) {
        flags["spaceHeld"] = false;
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
    int row = (bottomLeftY >= ypos) ? (bottomLeftY - ypos) / (pixelHeight/displayGrid.height) : -1;
    int col = (bottomLeftX <= xpos) ? (xpos - bottomLeftX) / (pixelWidth/displayGrid.width) : -1;
    if (row >= displayGrid.height) {
        row = -1;
    }
    if (col >= displayGrid.width) {
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
