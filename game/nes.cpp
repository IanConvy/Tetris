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
inputPtr{nullptr}, // Pointer to the InputHandler used for player inputs
filledRows{}, // Number of rows filled, used for the line clear animation
grid{20, 10}, // The grid used for the playfield
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
     * are Boolean variables which tell the game to try and perform 
     * certain actions on a given frame. These actions correspond to 
     * different kinds of game inputs available to the player, independent 
     * of any specific control scheme. The different commands are explained 
     * in more detail in the setCommands function.  
     */
    commands["doCCW"] =  false; // Attempt to rotate piece counterclockwise
    commands["doCW"] = false; // Attempt to rotate piece clockwise
    commands["doLeft"] = false; // Attempt to move piece left
    commands["doRight"] =  false; // Attempt to move piece right
    commands["softDrop"] =  false; // Attempt to drop piece at faster rate
    commands["leftDAS"] = false; // Attempt to move piece left with DAS
    commands["rightDAS"] = false; // Attempt to move piece right with DAS
    commands["reset"] = false; // Attempt to reset game


    /*
     * The "dynamic" variables represent game values that regularly change
     * between frames.   
     */

    /*
     * dropFrames records how many frames have elapsed since the piece was
     * last lowered by gravity. When it equals the "gravity" dynamic variable,
     * the piece is lowered again and the value is reset to zero.  
     */
    dynamic["dropFrames"] = 0;

    /*
     * The "gravity" variable determines how many frames need to elapse before
     * a piece falls one tile due to the effect of gravity. In other words, 
     * when dropFrames equals gravity, the piece falls. This value sets the 
     * overall speed of the game and is generally constant across a level, but it's 
     * included among the dynamic variables because a soft drop can temporarily 
     * descreases the value so that the piece falls faster.
     */
    dynamic["gravity"] = 0;

    /*
     * dasFrames records how many frames of Delayed Auto Scroll (DAS) have
     * been applied by the player. DAS allows a player to hold down a button and
     * make a piece continously move to either side, instead of having to repeatedly
     * tap the button (referred to as "hypertapping"). Once dasFrames has reached 
     * the limit specified by dasLimit, the piece is moved and dasFrames is reset to 
     * an intermediate quantity greater than zero set by dasFloor. The difference between 
     * dasFloor and dasLimit determines how fast the piece scrolls. To prevent the piece
     * from automatically scrolling when a button is first pressed, dasFrames is reset to 
     * zero and it must build up all the way back to dasLimit before the first scroll 
     * happens. After this first scroll the scrolling continues at the intermediate rate.  
     */
    dynamic["dasFrames"] = 0;

    /*
     * frozenFrames records how many frames have elapsed while the game is paused after 
     * a piece is placed (known as an "entry delay"). Once the number of frames reaches
     * the amount set by entryDelay, the next piece is loaded at the top of the board 
     * and the game is unpaused.
     */
    dynamic["frozenFrames"] = 0;

    /*
     * clearFrames records how many frames have elapsed while the game is paused during 
     * a line clear. Internally the line clear happens on the same frame that a piece is
     * placed, but the game performs a line clear animation for the player in which the 
     * blocks are removed in a sequence. During this animation the game is paused, just 
     * like for an entry delay. When frozenFrames reaches a set amount, the next piece 
     * is loaded at the top of the board and the game is unpaused. 
     *  
     */
    dynamic["clearFrames"] = 0;

    /*
     * totalFrames simply counts how many frames have elapsed since the game began. This
     * quantity is set back to zero when the game is reset.
     */
    dynamic["totalFrames"] = 0;

    /*
     * lineCount records the total number of lines that have been cleared, which
     * determines when the next level is reached. 
     */
    dynamic["lineCount"] =  0;

    // The score variable holds the player's score.  
    dynamic["score"] =  0;

    /*
     * The level variable holds the current level. It is initialized to whatever 
     * starting level is specified. 
     */
    dynamic["level"] = startLevel;

    // The flags are binary variables used internally to mark certain conditions.
    flags["frozen"] = false; // Indicates whether the game is paused for an entry delay 
    flags["dropDelay"] = true; // Indicates whether the first piece (with added delay) has fallen
    
    /*
     * The lineTypeCount vector holds the number of different line clears that have
     * ocurred. The first integer records the number of single line clears, the second
     * holds the double line clears, the third holds the triple line clears, and the 
     * fourth holds the number of Tetrises. 
     */
    lineTypeCount = {0, 0, 0, 0};

    filledRows.clear();
    grid.clear();
    currPiece = pieceGen.getRandomPiece();
    nextPiece = pieceGen.getRandomPiece();
    currPiece->setPosition(19, 5, 0); // Every piece starts with its center in the same position 
    setConstants();
    dynamic["gravity"] = constants["setGravity"];
}

void NESTetris::runFrame()
/*
 * The runFrame function is called to advance the game by one frame. Depending
 * on the state of the internal variables, a particular type of frame is then
 * chosen to be run. The InputHandler is queried every frame and the commands
 * are set, but they may or may not be used depending on the frame that's run. 
 */
{
    setCommands();
    if (commands["reset"]) {
        resetGame();
    }
    if (flags["frozen"] == true) {
        runFrozenFrame(); // Run during the entry delay
    }
    else {
        if (!filledRows.empty()) {
            runClearFrame(); // Run during line clears
        }
        else {
            runActiveFrame(); // Run during regular play
        }
    }
    resetBool(commands);
    ++ dynamic["totalFrames"];
}

void NESTetris::runFrozenFrame()
/*
 * The runFrozenFrame function is called during the entry delay that
 * occurs after a piece is placed. Nothing happens during these frames
 * except that the frozenFrames counter is incremented, and no commands 
 * are processed. After frozenFrames reaches the limit set by the entryDelay 
 * variable, a new piece is placed on the board and play resumes. 
*/
{
    ++ dynamic["frozenFrames"];
    if (dynamic["frozenFrames"] >= dynamic["entryDelay"]) {
        dynamic["frozenFrames"] = 0;
        flags["frozen"] = false;
        updatePiece();
        writePiece();
    }
}

void NESTetris::runClearFrame()
/*
 * The runClearFrame function is called while lines are being cleared and 
 * the game is paused. NES Tetris has a line clear animation that runs
 * for ~17 frames longer than the normal entry delay, with blocks being
 * removed across the cleared line roughly every 5 frames starting from 
 * the center of the board and moving outward. Once the animation is 
 * finished the next piece is loaded and play resumes. No commands are 
 * processed during this type of frame. 
 */
{
    ++ dynamic["clearFrames"];
    switch(dynamic["clearFrames"]) {
        case 7:
            for (int row : filledRows) {
                // Start from the middle and clear from each side 
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
    if (dynamic["clearFrames"] >= (17 + dynamic["entryDelay"])) {
        dynamic["clearFrames"] = 0;
        filledRows.clear();
        grid.swapGrids();
        updatePiece();
    }
}

void NESTetris::runActiveFrame()
/*
 * The runActiveFrame function is called during normal play (i.e. not paused).
 * The frame processes player input commands and moves the piece as needed,
 * detecting if it has been placed and whether lines have to be cleared.
 */
{   
    /*
     * The first thing the frame does is it clears the piece from the position it 
     * had on the previous frame, in preparation for possibly moving it to a new 
     * position.
     */
    clearPiece();

    /*
     * Next, the frame checks if the initial delay for the first piece 
     * placement is still in effect. The first piece does not drop right away in 
     * order to give player a chance to orient themselves. After a fixed amount of
     * time, or if the player does a soft drop, the piece begins to fall.
     */
    if (flags["dropDelay"] && dynamic["totalFrames"] >= constants["firstDelay"]) {
        flags["dropDelay"] = false;
    }

    /*
     * The frame then begins to process user commands. For piece movements, 
     * the pattern is faily consistent: the piece coordinates are shifted, 
     * and then the playfield grid is queried to determine if the new
     * position results in any collisions. If so, the movement is immediately 
     * reversed. When translating a piece left or right, a collision causes 
     * dasFrames to fill completely. 
     */

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
    if (commands["doLeft"]) { // Move without DAS
        dynamic["dasFrames"] = 0;
        currPiece->translate(0, -1);
        if (grid.collisionCheck(currPiece->coords)) {
            currPiece->translate(0, 1);
            dynamic["dasFrames"] = constants["dasLimit"];
        }
    }
    if (commands["leftDAS"]) { // Move with DAS
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
    if (commands["doRight"]) { // Move without DAS
        dynamic["dasFrames"] = 0;
        currPiece->translate(0, 1);
        if (grid.collisionCheck(currPiece->coords)) {
            currPiece->translate(0, -1);
            dynamic["dasFrames"] = constants["dasLimit"];
        }
    }
    if (commands["rightDAS"]) { // Move with DAS
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

    // Soft Drops
    /*
     * A "soft drop" is a player input that causes the piece to fall at 
     * a speed faster than the normal gravity of the level. In NES Tetris
     * the soft drop speed is equal to the speed at level 19, so after
     * level 18 the soft drop has no effect.
     */
    if (commands["softDrop"]) {
        dynamic["gravity"] = constants["dropGravity"];
        flags["dropDelay"] = false;
    }
    else {
        dynamic["gravity"] = constants["setGravity"];
    }

    /*
     * After processing commands, the frame then handles gravity. If the 
     * value of dropFrames equals the gravity value, the piece is lowered
     * and the frame checks for collisions. If a collision is found, then
     * the piece has landed and it locks into place. If any rows of the grid
     * have been filled, their positions are recorded, line counts and 
     * score are updated, and the next frame will run the line clear animation. 
     * Otherwise, an entry delay is started and freeze frames will be run next. 
     */

    if (!flags["dropDelay"] && dynamic["dropFrames"] >= dynamic["gravity"]) {
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
/*
 * This function fills the playfield grid with blocks corresponding to 
 * the current piece based on its coordinates.
 */
{
    grid.fillSet(currPiece->coords, currPiece->data.index);
}

void NESTetris::clearPiece()
/*
 * This function clears the blocks of the playfield grid corresponding to
 * the coordinates of the current piece. 
 */
{
    grid.fillSet(currPiece->coords, 0);
}

void NESTetris::updatePiece()
/*
 * This function sets nextPiece as the current piece and draws a random 
 * piece to become the new nextPiece.
 */
{
    currPiece.swap(nextPiece);
    nextPiece = pieceGen.getRandomPiece();
    currPiece->setPosition(19, 5, 0);
}

void NESTetris::checkLevel()
/*
 * This function checks to see if the level needs to be increased based on
 * the line count and level thresholds. 
 */
{
    if (dynamic["lineCount"] >= firstThreshold) {
        dynamic["level"] = startLevel + (dynamic["lineCount"] - firstThreshold)/10 + 1;
        setConstants();
    }
}

void NESTetris::setConstants()
/* 
 * This function sets values that are constant with respect to either a 
 * given level or the game as a whole. They are described individually in
 * more detail below. 
 */
{
    /*
     * dropGravity determines the gravity value when the player does a soft
     * drop. In NES Tetris this corresponds to a 1 frame delay unless the level
     * gravity is 0, in which case dropGravity is also assigned 0 (code further down).
     */
    constants["dropGravity"] = 1;

    /*
     * dasLimit determines how high dasFrame has to go to cause the piece
     * to move.   
     */
    constants["dasLimit"] = 15;

    /*
     * dasFloor determines what value dasFrame gets reset while the piece is scrolling.
     * This value is not zero, since DAS has a longer initial delay when a button is 
     * first pressed. The speed of scrolling is given by the difference between dasLimit
     * and dasFloor.
     */
    constants["dasFloor"] = 10;

    /*
     * firstDelay is the number of frames that the first piece will wait before dropping,
     * unless the player performs a soft drop
     */
    constants["firstDelay"] = 96;

    /*
     * The height and width of a Tetris grid is 20 x 10 in the vast majority of Tetris
     * versions, including NES Tetris.
     */
    constants["height"] = 20;
    constants["width"] = 10;

    /*
     * lineScore holds the number of points associated with each type of line clear. The
     * scaling is linear and consistent across all levels. 
     */
    lineScore = {
        40*(dynamic["level"] + 1),
        100*(dynamic["level"] + 1),
        300*(dynamic["level"] + 1),
        1200*(dynamic["level"] + 1)};
    
    /*
     * The following lines are used to product a value for setGravity, which determines
     * the gravity of a level when the player is not using soft drop. The gravity starts
     * at 47 frames and then decreases by 5 every level until level 9, where its assigned 
     * a value of 5. From level 10 to level 19 the gravity descreases by 1 frame every 
     * three levels, then from level 19 to level 28 the gravity remains at 1 frame. For
     * all levels greater than 28 the gravity is 0, which means the piece falls every frame.   
     */
    if (dynamic["level"] <= 8) constants["setGravity"] = 47 - 5*dynamic["level"];
    else if (dynamic["level"] == 9) constants["setGravity"] = 5;
    else if (dynamic["level"] > 9 && dynamic["level"] <= 18) constants["setGravity"] = 4 - (dynamic["level"]-10)/3;
    else if (dynamic["level"] > 18 && dynamic["level"] <= 28) constants["setGravity"] = 1;
    else if (dynamic["level"] > 28) {
        constants["setGravity"] = 0;
        constants["dropGravity"] = 0; // If dropGravity stayed at 1, it would be slower than normal gravity
    }
}

void NESTetris::setEntryDelay()
/*
 * This function determines the duration of the entry delay between piece
 * placements, which is determined by how high the piece was placed. Using
 * the center of the piece as reference, the first two rows (indexed from zero) 
 * have an entry delay of 9 frames, then row 2 has a delay of 11, then from row
 * 3 to row 14 the entry delay increases by 2 every four rows. Rows 14 and above 
 * have an entry delay of 17. 
 */
{
    int row = currPiece->centerRow;
    if (row <= 1) dynamic["entryDelay"] = 9;
    else if (row > 1 && row < 14) dynamic["entryDelay"] = 11 + 2*((row - 2)/4);
    else dynamic["entryDelay"] = 17;
}

void NESTetris::setCommands()
/*
 * This function retreives the state of the various keyboard keys used to
 * play and sets the game commands appropriately. The only complicated ones
 * are the left and right arrow keys which set commands for either a tap
 * or DAS depending on whether the key is being pressed for first time since
 * being released or whether it is being held down. The other keys simply
 * register the first press and ignore the key until it is released.  
 */
{
    auto keyMap = inputPtr->getStates({"a", "s", "left", "right", "down", "esc"});

    // A key:
    if (keyMap["a"] == "pressed" && keyMap["s"] == "off") {
        commands["doCCW"] = true;
    }

    // S key:
    if (keyMap["s"] == "pressed" && keyMap["a"] == "off") {
        commands["doCW"] = true;
    }

    // Left key:
    if (keyMap["left"] == "pressed" && keyMap["right"] == "off") {
        commands["doLeft"] = true;
    }
    if (keyMap["left"] == "held") {
        commands["leftDAS"] = true;
        }  

    // Right key:
    if (keyMap["right"] == "pressed" && keyMap["left"] == "off") {
        commands["doRight"] = true;
    }
    if (keyMap["right"] == "held") {
        commands["rightDAS"] = true;
    }

    // Down key:
    if (keyMap["down"] != "off") {
        commands["softDrop"] = true;
    }
    else {
        commands["softDrop"] = false;
    }

    // Escape key:
    if (keyMap["esc"] == "pressed") {
        commands["reset"] = true;
    }
}

void NESTetris::assignInput(InputHandler& inputSource)
/*
 * This function assigns an InputHandler from which the game can 
 * query inputs.  
 */
{
    inputPtr = &inputSource;
}

void resetBool(std::map<const std::string, bool>& bools)
/*
 * This function sets the Boolean values of a string/bool map to
 * false, which is convenenient when resetting the game commands. 
 */
{
    for (auto& keyValue : bools) {
        keyValue.second = false;
    }
}
