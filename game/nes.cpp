#include "headers/nes.hpp"

#include "headers/pieces.hpp"
#include "headers/grid.hpp"

#include <map>
#include <string>
#include <vector>
#include <memory>

/*
 * The NESTetris class is used to run a simulated version of NES Tetris,
 * with the game state being advanced using the "runFrame" method and all
 * internal variables being stored as members. The implementation details 
 * are described below. 
 */

NESTetris::NESTetris(int startLevel) :
/*
 * The constructor initializes many of the class members to null
 * values, which will later get actual assignements from the resetGame
 * method called in the body. The firstThreshold member is also assigned
 * in the constructor body, which marks the number of lines needed for 
 * the first level transition.  
 */

startLevel{startLevel}, // Sets the level to start the game at
firstThreshold{0}, // Sets the number of lines needed to advance from the first level
commands{}, // Map holding actions to be performed next frame, described more in resetGame
dynamic{}, // Map holding variables that change during play, described more in resetGame
flags{}, // Map holding binary state variables, described more in resetGame 
lineScore{0, 0, 0, 0}, // Holds the number of points to award for each type of line clear
lineTypeCount{0, 0, 0, 0}, // Holds the number of line clears performed for each type 
currPiece{nullptr}, // Pointer to the piece currently in play
nextPiece{nullptr}, // Pointer to the next piece (displayed in window)
inputPtr{nullptr}, // Pointer to the input map managed by an InputHandler instance
filledRows{}, // Number of rows filled, used for the cleared lines animation
grid{20, 10}, // The grid used as the playfield
// The generator used to create a random piece sequence
pieceGen{{"lPiece", "jPiece", "sPiece", "zPiece", "iPiece", "tPiece", "sqPiece"}}
{
    /*
     * Normally in NES Tetris the level advances every 10 line clears, however the 
     * first transition is treated differently depending on what level you start 
     * from. If you start from level 10 or lower, the game simply waits for you to 
     * get ten times that number and then continues advancing the level. However, 
     * for levels 11 through 15, the level transition is fixed at 100 line clears.
     * For every starting level greater than 15, an additional 10 line clears is 
     * added to the 100 line threshold (e.g. level 18 has a threshold of 130). 
     */
    if (startLevel <= 9) firstThreshold = 10*(startLevel + 1);
    else if (startLevel > 9 && startLevel <= 15) firstThreshold = 100;
    else firstThreshold = 10*(startLevel - 5);
    resetGame();
};

void NESTetris::resetGame()
/*
 * This function sets all of the state variables to the values 
 * they should have at the start of a game. Each variable is 
 * discussed below.
 */
{
    /*
     * These first set of variables are called "commands", as they
     * are Boolean variables which tell the game to perform certain
     * action on a given active frame. Most are activated by user 
     * inputs, while some are managed internally.  
     */
    commands["doCCW"] =  false; // Attempt to rotate piece counterclockwise
    commands["doCW"] = false; // Attempt to rotate piece clockwise
    commands["doLeft"] = false; // Attempt to move piece left
    commands["doRight"] =  false; // Attempt to move piece right
    commands["softDrop"] =  false; // Attempt to drop piece at faster rate
    commands["leftDAS"] = false; // Attempt to move piece left with DAS
    commands["rightDAS"] = false; // Attempt to move piece ri
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
    if (inputPtr->getState("a") == "pressed" && inputPtr->getState("s") == "off") {
        commands["doCCW"] = true;
    }
    // S key:
    if (inputPtr->getState("s") == "pressed" && inputPtr->getState("a") == "off") {
        commands["doCW"] = true;
    }
    // Left key:
    if (inputPtr->getState("left") == "pressed" && inputPtr->getState("right") == "off") {
        commands["doLeft"] = true;
        commands["clearDAS"] = true;
    }
    else if (inputPtr->getState("left") == "held") {
            commands["leftDAS"] = true;
        }    
    // Right key:
    if (inputPtr->getState("right") == "pressed" && inputPtr->getState("left") == "off") {
        commands["doRight"] = true;
        commands["clearDAS"] = true;
    }
    else if (inputPtr->getState("right") == "held") {
        commands["rightDAS"] = true;
    }        
    // Down key:
    if (inputPtr->getState("down") != "off") {
        commands["softDrop"] = true;
    }
    else {
        commands["softDrop"] = false;
    }
    // Escape key:
    if (inputPtr->getState("esc") == "pressed") {
        commands["reset"] = true;
    }
}

void NESTetris::assignInput(InputHandler& inputSource)
{
    inputPtr = &inputSource;
}

void resetBool(std::map<const std::string, bool>& bools)
{
    for (auto& keyValue : bools) {
        keyValue.second = false;
    }
}
