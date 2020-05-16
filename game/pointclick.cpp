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

/*
 * The PointClick class is used to run a version of Tetris in which there is no gravity,
 * with pieces being placed manually by the player at their own pace. The game keeps a 
 * record of the piece sequence and the moves that are made, allowing the player to review
 * past moves and try a different build pattern. This mode also has an associated AI that
 * can analyze the current state of the board and recommend ideal moves. The implementation
 * details of all of this are described below. 
 */

PointClick::PointClick(int startLevel) :
/*
 * The constructor initializes many of the class members to null
 * values, which will later get actual assignements from the resetGame
 * method called in the body. The firstThreshold member is also assigned
 * in the constructor body, which marks the number of lines needed for 
 * the first level transition. The constructor takes as argument a set
 * of coordinates which mark the four corners of the playfield. These
 * values are used by the game to determine how the position of the 
 * mouse lines up with the grid of the playfield. 
 */

startLevel{startLevel}, // Sets the level to start the game at
firstThreshold{0}, // Sets the number of lines needed to advance from the first level
windowHeight{0}, // Holds the height of the window
windowWidth{0}, // Holds the width of the window
playHeight{0}, // Holds the height of the playfield
playWidth{0}, // Holds the width of the playfield
bottomLeftX{0}, // Holds the x-coordinate of the bottom left corner of the playfield
bottomLeftY{0}, // Holds the y-coordinate of the bottom left corner of the playfield
commands{}, // Map holding actions to be performed next frame, described more in resetGame
dynamic{}, // Map holding variables that change during play, described more in resetGame
flags{}, // Map holding binary state variables, described more in resetGame
record{}, // Vector of Boards that keeps a record of past moves
pieceSeq{}, // Vector of piece names that holds the game's sequence of pieces
lineScore{0, 0, 0, 0}, // Holds the number of points to award for each type of line clear
currPiece{nullptr}, // Pointer to the piece currently in play
nextPiece{nullptr}, // Pointer to the next piece (displayed in window)
inputPtr{nullptr}, // Pointer to the InputHandler used for player inputs
eval{}, // Map that holds the evaluation metrics returned by the AI 
board{20, 10}, // Board used during play
boardBackup{20, 10}, // Board used to store the playfield before switching to AI mode
displayGrid{20, 10}, // Grid used by the Drawer to display the playfield
// The generator used to create a random piece sequence
pieceGen{{"lPiece", "jPiece", "sPiece", "zPiece", "iPiece", "tPiece", "sqPiece"}},
evaluator{20, 10, 5, 0, 5} // AI instance used to analyze and offer moves
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

void PointClick::resetGame()
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
    commands["reset"] = false; // Attempt to reset game
    commands["placePiece"] = false; // Attempt to place piece
    commands["recordBack"] = false; // Attempt to load previous board position
    commands["recordForward"] = false; // Attempt to load next board position
    commands["enterAIMode"] = false; // Attempt to enter AI evaluation mode
    commands["exitAIMode"] = false; // Attempt to exist AI evaluation mode
    commands["evalForward"] = false; // Attempt to view the next AI recommended move
    commands["evalBackward"] = false; // Attempt to view the previous AI recommended move

    /*
     * The "dynamic" variables represent game values that regularly change
     * between frames.   
     */

     // The score variable holds the player's score.
    dynamic["score"] =  0;

    /*
     * The level variable holds the current level. It is initialized to whatever 
     * starting level is specified. 
     */
    dynamic["level"] = startLevel;

    /*
     * mouseRow and mouseCol hold the position that the mouse is currently 
     * located in. It is calculated using the pixel position of the mouse 
     * as reported by the InputHandler. A value of -1 is used when the mouse 
     * is positioned outside of the playfield. 
     */
    dynamic["mouseRow"] = -1;
    dynamic["mouseCol"] = -1;

    /*
     * lastPlacedRow and lastPlacedCol records the position of the block at the center 
     * of the last piece that was placed, and holds on to it until the mouse is moved 
     * to a different position. This is done so that the game avoids highlighting the
     * next piece immediately after the current piece is placed, giving the player an 
     * unobstructed view of their placement until the cursor is moved. A value of -1 is
     * set after the cursor is moved and there is no longer any need to keep the position.    
     */
    dynamic["lastPlacedRow"] = -1;
    dynamic["lastPlacedCol"] = -1;

    /*
     * The move variable hold the current move of the game. This value increments whenever 
     * a piece is placed, and can be shifted back and forth as the player reviews old moves. 
     */
    dynamic["move"] = 0;

    /*
     * evalIndex holds the index of the AI-recommended move currently being viewed by the
     * player. The moves are ordered based on how favorably the AI has evaluated them, arranged
     * from best to worst. 
     */
    dynamic["evalIndex"] = 0;
    
    // The flags are binary variables used internally to mark certain conditions.
    flags["aiMode"] = false; // Indicates whether the player is viewing AI-recommended moves
    flags["inBounds"] = false; // Indicates if the mouse is positioned within the playfield

    pieceSeq = pieceGen.getRandomSequence(750);
    updatePiece();
    setConstants();
    board.reset();
    displayGrid.clear();
    record.clear();
    record.push_back(board);
    eval = (true /*getHoles(board.grid) > 0 // BROKEN WHEN SWITCHING GRID*/ ) ? burnEval(board, evaluator.well) : positionEval(board, evaluator.well);
    evaluator.setGravity(constants["setGravity"]);
}

void PointClick::runFrame()
/*
 * The runFrame function is called to advance the game by one frame. Depending
 * on the state of the internal variables, a particular type of frame is then
 * chosen to be run. The InputHandler is queried every frame and the commands
 * are set, but they may or may not be used depending on the frame that's run. 
 */
{
    unHighlightPiece();
    checkWindow();
    checkMouse();
    setCommands();
    if (commands["reset"]) {
        resetGame();
    }
    
    if (flags["aiMode"] || commands["enterAIMode"]) { 
        runAIFrame(); // Run while in AI mode
    }
    else {
        runGameFrame(); // Run during normal play
    }

    // Clear commands:
    for (auto& keyValue : commands) {
        keyValue.second = false;
    }
}

void PointClick::runAIFrame()
/*
 * The runAIFrame function is called when the player is looking at 
 * moves being analyzed and recommended by the AI. The player is not
 * able to manipulate any pieces or review past moves during an AI
 * frame, but they are able to scroll through the different AI move
 * recommendations and use one as their next move. By repeatedly 
 * selecting the best move, the player can watch the AI play a game 
 * of Tetris.
 */
{
    /*
     * The first thing the frame does is check to see if the player had 
     * activated AI mode on this frame, in which case it tells the AI
     * to generate moves, creates a backup of the playfield board, and
     * starts displaying the generated moves.  
     */
    if (commands["enterAIMode"]) {
        flags["aiMode"] = true;
        dynamic["evalIndex"] = 0;
        boardBackup = board;
        evaluator.generateMoves(board, *currPiece);
        displayEvalMoves(dynamic["evalIndex"]);
    }

    /*
     * The frame then processes any allowed input commands. The player is
     * able to tell the game to display different AI-recommended moves by 
     * scrolling through the evaluation list, and they are able to choose
     * one of the moves to play. After a move is played, the AI generates
     * a new set of moves based on the updates board and the process repeats. 
     */
    if (!evaluator.moves.empty()) {
        // Browse different eval moves:
        if (commands["evalBackward"]) {
            dynamic["evalIndex"] = (dynamic["evalIndex"] > 0) ? dynamic["evalIndex"] - 1 : evaluator.moves.size() - 1;
            displayEvalMoves(dynamic["evalIndex"]);
        }
        else if (commands["evalForward"]) {
            dynamic["evalIndex"] = (dynamic["evalIndex"] < evaluator.moves.size() - 1 ) ? dynamic["evalIndex"] + 1 : 0;
            displayEvalMoves(dynamic["evalIndex"]);
        }
        // Place piece based on AI suggestion:
        if (commands["placeAIPiece"]) {
            board = evaluator.moves[dynamic["evalIndex"]].first;
            boardBackup = board; // The backup is updated to account for the new move
            nextMove();
            evaluator.generateMoves(board, *currPiece);
            dynamic["evalIndex"] = 0;
            displayEvalMoves(dynamic["evalIndex"]);
        }
    }

    /*
     * Finally, the frame checks to see if the player wants to exit AI mode, in 
     * which case the backup board is copied to the main board, the display is
     * redrawn from the backup, and the AI evaluates the position. 
     */
    if (commands["exitAIMode"]) {
        board = boardBackup;
        displayGrid = boardBackup.grid;
        eval = (true /*getHoles(board.grid) > 0 // BROKEN WHEN SWITCHING GRID*/ ) ? burnEval(board, evaluator.well) : positionEval(board, evaluator.well);
        flags["aiMode"] = false;
    }
}

void PointClick::runGameFrame()
/*
 * The runGameFrame function is called during normal gameplay (i.e. no AI mode).
 * Since the games does not set any inherent pace for the player, the frame simply
 * waits for a command and carries out the desired action if it is allowed.  
 */
{
    // Browse move record:
    if (commands["recordBack"] && dynamic["move"] > 0) {
        readMove(dynamic["move"] - 1);
    }
    else if (commands["recordForward"] && dynamic["move"] < record.size() - 1) {
        readMove(dynamic["move"] + 1);
    }
    // Move piece:
    if (flags["inBounds"]) {
        currPiece->setPosition(dynamic["mouseRow"], dynamic["mouseCol"], currPiece->orient);
    }
     // Counterclockwise rotation:
    if (commands["doCCW"]) {
        currPiece->rotate(-1);
    }
    // Clockwise rotation:
    if (commands["doCW"]) {
        currPiece->rotate(1);
    }
    // Place piece:
    if (commands["placePiece"] && flags["inBounds"]) {
        if (!board.grid.collisionCheck(currPiece->coords)) {
            dynamic["lastPlacedRow"] = dynamic["mouseRow"];
            dynamic["lastPlacedCol"] = dynamic["mouseCol"];
            board.placePiece(*currPiece);
            nextMove();
            currPiece->setPosition(dynamic["mouseRow"], dynamic["mouseCol"], 0);
        }
    }

    /*
     * After processing the commands, the frame displays the current piece so 
     * that the player can move it to a desired position. The piece is highlighted
     * with a solid color to visually distinguish it from pieces that have already
     * been placed, with blue indicating that the piece is in an allowed position 
     * and red indicating that the position is invalid due to collisions.  
     */
    bool moved = dynamic["mouseRow"] != dynamic["lastPlacedRow"] || dynamic["mouseCol"] != dynamic["lastPlacedCol"];
    if (flags["inBounds"] && moved) {
        bool collision = board.grid.collisionCheck(currPiece->coords);
        highlightPiece(collision);
        dynamic["lastPlacedRow"] = -1;
        dynamic["lastPlacedCol"] = -1;
    }
}

void PointClick::highlightPiece(bool collision)
/*
 * This function fills the blocks of the display grid based on
 * the coordinates of the current piece. The collision Boolean 
 * indicates whether the piece should be highlighted in blue 
 * or red. 
 */
{
    if (!collision) {
        displayGrid.fillSet(currPiece->coords, 8);
    }
    else {
        displayGrid.fillSet(currPiece->coords, 9);
    }
}

void PointClick::unHighlightPiece()
/*
 * This function resets the blocks of the display grid corresponding 
 * to the coordinates of the previous piece, returning them to the 
 * values in the game Board. As its name suggests, the purpose of this
 * function is to remove the highlighting caused by highlightPiece.
 */
{
    for (auto rowCol : currPiece->coords) {
        displayGrid.fill(rowCol[0], rowCol[1], board.grid.get(rowCol[0], rowCol[1]));
    }
}

void PointClick::nextMove()
/*
 * This function advances the game to the next move. Aside from updating 
 * the score and selecting the next piece, the function determines whether
 * the player is making a new move based on a previous position (i.e. they 
 * went back to review an older move and decided to make a different move). 
 * If so, the move record gets truncated to the point at which player branched 
 * off, with the new move being the most recent move on record. 
 */
{
    if (dynamic["move"] != record.size() - 1) {
        truncateRecord(dynamic["move"]  + 1);
    }
    ++dynamic["move"];
    record.push_back(board);
    displayGrid = board.grid;
    updateScore();
    updateLevel();
    updatePiece();
    eval = (true /*getHoles(board.grid) > 0 // BROKEN WHEN SWITCHING GRID*/ ) ? burnEval(board, evaluator.well) : positionEval(board, evaluator.well);
}

void PointClick::readMove(int move)
/*
 * This function loads a Board from the move record onto the
 * playfield, allowing the history of the game to be browsed
 * or for the player to change their mind and make a different 
 * move. 
 */
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
/*
 * This function erases moves from the move record based on the
 * passed index, wih that move and more recent moves all being
 * deleted.
 */
{
    record.erase(record.begin() + moveInclusive, record.end()); 
}

void PointClick::displayEvalMoves(unsigned int move_index)
/*
 * This function sets the display grid to display a move from
 * the set of AI-recommended moves based on the passed index.
 */
{
    if (move_index < evaluator.moves.size()) {
        auto move = evaluator.moves[move_index];
        displayGrid = move.first.grid;
        board = move.first;
        eval = (true /*getHoles(board.grid) > 0 // BROKEN WHEN SWITCHING GRID*/ ) ? burnEval(board, evaluator.well) : positionEval(board, evaluator.well);
    }
}

void PointClick::updatePiece()
/*
 * This function selects the current piece and the next piece 
 * from the piece sequence based on the current move. 
 */
{
    currPiece = pieceGen.getPiece(pieceSeq[dynamic["move"]]);
    nextPiece = pieceGen.getPiece(pieceSeq[dynamic["move"] + 1]);
}

void PointClick::updateScore()
/*
 * This function updates the score based on the line count of the
 * Board and the per-line scores dictated by the level. 
 */
{
    dynamic["score"] = 
        lineScore[0] * board.lineTypeCount[0] +
        lineScore[1] * board.lineTypeCount[1] +
        lineScore[2] * board.lineTypeCount[2] +
        lineScore[3] * board.lineTypeCount[3];
}

void PointClick::updateLevel()
/*
 * This function checks to see if the level needs to be increased based on
 * the line count and level thresholds. 
 */
{
    if (board.lineCount >= firstThreshold) {
        dynamic["level"] = startLevel + (board.lineCount - firstThreshold)/10 + 1;
        setConstants();
    }
}

void PointClick::checkWindow()
/*
 * This function queries the InputHandler to determine the current size of the window, 
 * then checks to see if the window size has been changed. If so, the height, width,
 * and bottom corner position of the playfield are all recalculated so that the game
 * can accurately match the pixel position of the mouse to the position on the grid.
 * The seemingly arbitrary values used in the equaltion are simply a set of absolute
 * pixel sizes, measured from the top left corner, that correctly describe the 
 * proportions of the NES Tetris board graphics:
 *      899.0: Height of the graphics
 *      1035.0: Width of the graphics
 *      159.0: Top of the playfield section
 *      805.0: Bottom of the playfield section
 *      390.0: Left side of the playfield section
 *      710.0: Right side of the playfield section   
 */
{
    std::vector<int> pixelHeightWidth = inputPtr->getWindowSize();
    if (pixelHeightWidth[0] != windowHeight || pixelHeightWidth[1] != windowWidth) { 
        windowHeight = pixelHeightWidth[0];
        windowWidth = pixelHeightWidth[1];
        double heightScale =  windowHeight / 899.0; // Ratio of window size to absolute size                   
        double widthScale = windowWidth / 1035.0; // Ratio of window size to absolute size 
        playHeight = heightScale * (805.0 - 159.0);
        playWidth = widthScale * (710.0 - 390.0);
        bottomLeftX = 390.0 * widthScale;
        bottomLeftY = 805.0 * heightScale;
    }
}

void PointClick::checkMouse()
/*
 * This function determines whether the mouse cursor is located in the
 * playfield, and if so which block it is closest to. The coordinates of
 * this block are then assigned to the mouseRow and mouseCol variables.
 */
{
    std::vector<double> xyPos = inputPtr->getMousePos(); 
    auto mouseRowCol = getGridPosition(xyPos[0], xyPos[1]);
    dynamic["mouseRow"] = mouseRowCol[0];
    dynamic["mouseCol"] = mouseRowCol[1];
    if (mouseRowCol[0] >= 0 && mouseRowCol[0] < displayGrid.height && mouseRowCol[1] >= 0 && mouseRowCol[1] < displayGrid.width) {
        flags["inBounds"] = true;
    }
    else{
        flags["inBounds"] = false;
    }
}

void PointClick::setConstants()
/* 
 * This function sets values that are constant with respect to either a 
 * given level or the game as a whole. They are described individually in
 * more detail below. 
 */
{
    /*
     * The height and width of a Tetris grid is 20 x 10 in the vast majority of Tetris
     * versions, including NES Tetris.
     */
    constants["width"] = 10;
    constants["height"] = 20;

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
    else if (dynamic["level"] > 28) constants["setGravity"] = 0;
}

void PointClick::setCommands()
/*
 * This function retreives the state of the various keyboard keys and mouse 
 * buttons used to play and sets the game commands appropriately. In all cases
 * the game simply register the first press and ignore the button until it is 
 * released.  
 */
{   
    auto keyStates = inputPtr->getStates({"mouseLeft", "mouseRight", "a", "s", 
        "z", "x", "left", "right", "up", "down", "space", "esc"});

    // Left mouse button:
    if (keyStates["mouseLeft"] == "pressed") {
        commands["placePiece"] = true;
    }

    // Right mouse button:
    if (keyStates["mouseRight"] == "pressed") {
        commands["doCW"] = true;
    }

    // A key:
    if (keyStates["a"] == "pressed" && keyStates["s"] == "off") {
        commands["doCCW"] = true;
    }

    // S key:
    if (keyStates["s"] == "pressed" && keyStates["a"] == "off") {
        commands["doCW"] = true;
    }

    // Z key:
    if (keyStates["z"] == "pressed") {
        commands["recordBack"] = true;
    }

    // X key:
    if (keyStates["x"] == "pressed") {
        commands["recordForward"] = true;
    }

    // Left key:
    if (keyStates["left"] == "pressed") {
        commands["evalBackward"] = true;
    }

    // Right key:
    if (keyStates["right"] == "pressed") {
        commands["evalForward"] = true;
    }

    // Up key:
    if (keyStates["up"] == "pressed") {
        commands["enterAIMode"] = true;
    }

    // Down key:
    if (keyStates["down"] == "pressed") {
        commands["exitAIMode"] = true;
    }

    // Space bar:
    if (keyStates["space"] == "pressed") {
        commands["placeAIPiece"] = true;
    }

    // Escape key:
    if (keyStates["esc"] == "pressed") {
        commands["reset"] = true;
    }
}

std::vector<int> PointClick::getGridPosition(double xpos, double ypos)
/*
 * This function determines which grid block the mouse cursor is closest to. 
 * The location of this block is found by getting the mouse position from 
 * the InputHandler and then getting its distance from the bottom left 
 * corner of the playfield. The distance-per-block is calculated by dividing 
 * the pixel height and width of the playfield by the block height and width 
 * of the grid. Dividing the mouse distance by the distance-per-block gives
 * the nearest block.  
 */
{
    int row = (bottomLeftY >= ypos) ? (bottomLeftY - ypos) / (playHeight/displayGrid.height) : -1;
    int col = (bottomLeftX <= xpos) ? (xpos - bottomLeftX) / (playWidth/displayGrid.width) : -1;
    if (row >= displayGrid.height) {
        row = -1;
    }
    if (col >= displayGrid.width) {
        col = -1;
    }
    return std::vector<int>{row, col};
}

void PointClick::assignInput(InputHandler& inputSource)
/*
 * This function assigns an InputHandler from which the game can 
 * query inputs.  
 */
{
    inputPtr = &inputSource;
}
