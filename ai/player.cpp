#include "headers/player.hpp"

#include "../game/headers/pieces.hpp"
#include "../game/headers/grid.hpp"
#include "../game/headers/board.hpp"
#include "headers/tools.hpp"

#include <vector>
#include <memory>

Player::Player(int startLevel) : 
startLevel{startLevel},
level{startLevel},
firstThreshold{0},
score{0},
gravity{0},
lineScore{0, 0, 0, 0},
evaluator{20, 10, 5, 0, 5},
pieceGen{{"lPiece", "jPiece", "sPiece", "zPiece", "iPiece", "tPiece", "sqPiece"}},
currPiece{nullptr},
nextPiece{nullptr},
board{20, 10},
eval{}
{
    if (startLevel <= 9) firstThreshold = 10*(startLevel + 1);
    else if (startLevel > 9 && startLevel <= 15) firstThreshold = 100;
    else firstThreshold = 10*(startLevel - 5);
    reset();
}

void Player::reset()
{
    currPiece = pieceGen.getRandomPiece();
    nextPiece = pieceGen.getRandomPiece();
    score = 0;
    level = startLevel;
    board.reset();
    setConstants();
}

void Player::updateScore()
{
    score = 
        lineScore[0] * board.lineTypeCount[0] +
        lineScore[1] * board.lineTypeCount[1] +
        lineScore[2] * board.lineTypeCount[2] +
        lineScore[3] * board.lineTypeCount[3];
}

void Player::updateLevel()
{
    if (board.lineCount >= firstThreshold) {
        level = startLevel + (board.lineCount - firstThreshold)/10 + 1;
        setConstants();
    }
}

void Player::setConstants()
{
    lineScore = {
        40*(level + 1),
        100*(level + 1),
        300*(level + 1),
        1200*(level + 1)};
    if (level <= 8) gravity = 47 - 5*level;
    else if (level == 9) gravity = 5;
    else if (level > 9 && level <= 18) gravity = 4 - (level-10)/3;
    else if (level > 18 && level <= 28) gravity = 1;
    else if (level > 28) gravity = 0;
    evaluator.setGravity(gravity);
}

void Player::newPiece()
{
    currPiece.swap(nextPiece);
    nextPiece = pieceGen.getRandomPiece();
}

bool Player::nextMove()
{
    bool topout = false;
    evaluator.generateMoves(board, *currPiece);
    if (!evaluator.moves.empty()) {
        board = evaluator.moves[0].first;
        newPiece();
        updateLevel();
        setConstants();
        updateScore();
    }
    else {
        topout = true;
    }
    return topout;
}


