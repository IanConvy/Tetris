#include "headers/pieces.hpp"

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <random>

/*
 * The PieceData class holds all of the information that characterizes a
 * piece, as described below. One instance is created for each piece type,
 * with all of the different instances of the Piece class containing constant
 * refences to one of them.  
 */

PieceData::PieceData(
    /* 
     * The name of the piece, used as an identifier in piece sequences and 
     * as the key in the master pieceData map allPieces.
     */
    std::string name,

    /*
     * The pieceIndex is how the type of piece is specified on a Grid object,
     * since the name is impractical to store directly. 
     */
    const unsigned int pieceIndex,

    /*
     * numOrients is simply the number of ways that a piece can be rotated, 
     * which therefore means it gives the size of all of the vector-of-vector
     * members described below. 
     */
    const int numOrients,

    /*
     * The coordOffsets are a set of nested vectors describing the full shape of
     * the piece. The inner-most vector holds a pair of integers that describes 
     * the location of a piece block relative to the "center". During play the 
     * "center" is assigned a (row, col) coordinate, and piece blocks are then 
     * built around it by adding the offset to that coordinate. The number of 
     * integer pairs determines the number of blocks in the piece, while the 
     * outermost vector holds a set of integer pairs for each orientation of 
     * the piece. 
     */
    std::vector<std::vector<std::vector<int>>> coordOffsets,

    /*
     * bottomSurf describes the contour of the bottom of the piece using a 
     * set of integers that denote how much the higher a given block is 
     * relative to the block next to it. For example, the square piece has 
     * a flat bottom and therefore its contour is represented by 0, while the
     * z-piece has a step shape represented by 1. These contours are used to
     * determine whether a given piece will fit cleanly onto an existing stack
     * of pieces. There are a set of integers for each orientation. 
     * 
     */
    std::vector<std::vector<int>> bottomSurf,

    /*
     * topSurf is the same as bottomSurf except that it describes the contout of 
     * the top of the piece instead of the bottom. 
     */
    std::vector<std::vector<int>> topSurf,

    /*
     * sideHeight describes how much the edges of a piece alter the contour of the 
     * surface it is placed on. For example, the square piece is two blocks tall, so
     * its left side height is 2 (two blocks heigher than the block to its left) and
     * its right side height it -2 (the surface contour drops by two after the piece).
     */
    std::vector<std::vector<int>> sideHeights) :
name{name}, 
index{pieceIndex},
numOrients{numOrients},
coordOffsets{coordOffsets},
bottomSurf{bottomSurf},
topSurf{topSurf},
sideHeights{sideHeights}
{}

// Default constructor allows for easy creation of dummy piece
PieceData::PieceData() :
name{},
index{0},
numOrients{1},
coordOffsets{{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}}},
bottomSurf{{}},
topSurf{{}},
sideHeights{{1, -1}}
{}


/*
 * The Piece class represents a Tetris piece during play. The identity 
 * of the piece is determined by the pieceData that it is assigned when 
 * instantiated. The class members hold to position of the piece at any
 * given instant, and provide function to move and rotate the piece.    
 */

Piece::Piece(const PieceData& data) :
data{data}, // pieceData reference that defines the piece type
centerRow{0}, // The row assigned as the piece's "center"
centerCol{0}, // The column assigned as the piece's "center"
orient{0}, // The orientation of the piece clockwise from entry orientation
coords{{0, 0}, {0, 0}, {0, 0}, {0, 0}} // Absolute coordinates for piece
{}

// Default constructor allows for easy creation of dummy piece
Piece::Piece() :
data{nullPiece},
centerRow{0},
centerCol{0},
orient{0},
coords{{0, 0}, {0, 0}, {0, 0}, {0, 0}}
{}

void Piece::setPosition(int newCenterRow, int newCenterCol, unsigned int newOrient)
/*
 * This function sets the center position and orientation of the pieces
 * based on the passed arguments. The piece coordinates are then calculated
 * using the offsets defined by the referenced pieceData instance. 
 */
{
    centerRow = newCenterRow;
    centerCol = newCenterCol;
    orient = newOrient;
    for (int i = 0; i < data.coordOffsets[orient].size(); ++i) {
        coords[i][0] = centerRow + data.coordOffsets[orient][i][0]; // Row
        coords[i][1] = centerCol + data.coordOffsets[orient][i][1]; // Column
    }
}

void Piece::rotate(int turns)
/*
 * This function rotates the piece by turns*(90 deg), with a positive value
 * rotating clockwise and a negative value counterclockwise. By definition,
 * a rotation does not alter the position of the center. 
 */
{
    // Get the new orientation relative to 0, can be postive or negative 
    int newOrientSigned = (orient + turns) % data.numOrients;
    // If newOrientSigned is negative, loop back around from largest orientation
    int newOrient = (newOrientSigned >= 0) ? newOrientSigned : data.numOrients + newOrientSigned;
    setPosition(centerRow, centerCol, newOrient);
}

void Piece::translate(int dRow, int dCol)
// This function shifts the center position based on the passed arguments.
{
    int newCenterRow = centerRow + dRow;
    int newCenterCol = centerCol + dCol;
    setPosition(newCenterRow, newCenterCol, orient);
}

/*
 * The PieceGenerator class is used to get new Tetris pieces. It serves as 
 * the interface between the game and the piece data. The class can
 * retrieve pieces by name or generate random piece sequences based 
 * on a piece list passed to the constructor. The pieces are all managed 
 * using unique pointers. 
 */

PieceGenerator::PieceGenerator(std::vector<std::string> pieceList) :
uDistr(0, pieceList.size() > 0 ? pieceList.size() - 1 : 0),
pieceList{pieceList}
{
    std::random_device rDev; // System-defined true random-number generator
    rEng.seed(rDev()); // Seed the psuedo-random engine, which will create the random sequences
}

std::unique_ptr<Piece> PieceGenerator::getRandomPiece()
/*
 * This function picks a piece name at random from the generator's internal piece
 * list. It then uses this name to retrieve the piece data from the allPieces map,
 * returning a null piece if there is no matching name.
 */
{
    int index = uDistr(rEng); // Use the psuedo-random generator to uniformly sample an index
    auto pieceItr = allPieces.find(pieceList[index]);
    std::unique_ptr<Piece> randPiece{pieceItr != allPieces.end() ? new Piece(pieceItr->second) : new Piece()};
    return randPiece;
}

std::unique_ptr<Piece> PieceGenerator::getPiece(std::string pieceName)
/*
 * This function simply retrieves the piece whose name matches the passed
 * argument, or a null piece if no matching name is found. Note that this
 * piece does not have to be in the generator's piece list, just in the 
 * allPieces map. 
 */
{
    auto pieceItr = allPieces.find(pieceName);
    std::unique_ptr<Piece> piece{pieceItr != allPieces.end() ? new Piece(pieceItr->second) : new Piece()};
    return piece;
}

std::vector<std::string> PieceGenerator::getRandomSequence(int length)
/*
 * This function generates a random piece sequence of the specified length using
 * names drawn from the generator's piece list. It returns only a vector of names,
 * without creating any pieces. 
 */
{
    std::vector<std::string> sequence;
    sequence.reserve(length);
    for (int i = 0; i < length; ++i) {
        int index = uDistr(rEng);
        sequence.push_back(pieceList[index]);
    }
    return sequence;
}

const PieceData nullPiece{}; // Used as a default when a non-existent piece is requested


// The master piece map, which holds the pieceData instances indexed by name
const std::map<const std::string, PieceData> allPieces{
    {"lPiece", {
        "lPiece",
        1, // Index
        4, // Number of orientations
        // Offsets from center
        {{{0, 0}, {0, 1}, {0, -1}, {-1, -1}},
        {{0, 0}, {-1, 0}, {1, 0}, {1, -1}},
        {{0, 0}, {0, -1}, {0, 1}, {1, 1}},
        {{0, 0}, {1, 0}, {-1, 0}, {-1, 1}}},
        // Bottom Surface
        {{1, 0}, {-2}, {0, 0}, {0}},
        // Top Surface
        {{0, 0}, {0}, {0, 1}, {-2}},
        // Side Heights
        {{2, -1}, {1, -3}, {1, -2}, {3, -1}}
        }
    },
    {"jPiece", {
        "jPiece",
        2, // Index
        4, // Number of orientations
        // Offsets from center
        {{{0, 0}, {0, -1}, {0, 1}, {-1, 1}},
        {{0, 0}, {1, 0}, {-1, 0}, {-1, -1}},
        {{0, 0}, {0, 1}, {0, -1}, {1, -1}},
        {{0, 0}, {-1, 0}, {1, 0}, {1, 1}}}, 
        // Bottom Surface
        {{0, -1}, {0}, {0, 0}, {2}},
        // Top surface
        {{0, 0}, {2}, {-1, 0}, {0}},
        // Side Heights
        {{1, -2}, {1, -3}, {2, -1}, {3, -1}}
        }   
    },
    {"sPiece", {
        "sPiece",
        3, // Index
        2, // Number of orientations
        // Offsets from center
        {{{0, 0}, {0, 1}, {-1, -1}, {-1, 0}},
        {{0, 0}, {1, 0}, {0, 1}, {-1, 1}}},
        // Bottom Surface
        {{0, 1}, {-1}},
        // Top Surface
        {{1, 0}, {-1}},
        // Side Heights
        {{1, -1}, {2, -2}}
        }
    },
    {"zPiece", {
        "zPiece",
        4, // Index
        2, // Number of orientations
        // Offsets from center
        {{{0, 0}, {0, -1}, {-1, 0}, {-1, 1}},
        {{0, 0}, {-1, 0}, {0, 1}, {1, 1}}},
        // Bottom Surface
        {{-1, 0}, {1}},
        // Top Surface
        {{0, -1}, {1}},
        // Side Heights
        {{1, -1}, {2, -2}}
        }
    },
    {"iPiece", {
        "iPiece",
        5, // Index
        2, // Number of orientations
        // Row offsets from center
        {{{0, 0}, {0, -2}, {0, -1}, {0, 1}},
        {{0, 0}, {2, 0}, {1, 0}, {-1, 0}}},
        // Bottom Surface
        {{0, 0, 0, 0}, {}},
        // Top Surface
        {{0, 0, 0, 0}, {}},
        // Side Heights
        {{1, -1}, {4, -4}}
        }
    },
    {"tPiece", {
        "tPiece",
        6, // Index
        4, // Number of orientations
        // Offsets from center
        {{{0, 0}, {-1, 0}, {0, 1}, {0, -1}},
        {{0, 0}, {0, -1}, {-1, 0}, {1, 0}},
        {{0, 0}, {1, 0}, {0, -1}, {0, 1}},
        {{0, 0}, {0, 1}, {1, 0}, {-1, 0}}},
        // Bottom Surface
        {{-1, 1}, {-1}, {0, 0}, {1}},
        // Top Surface
        {{0, 0}, {1}, {1, -1}, {-1}},
        // Side Heights
        {{1, -1}, {1, -3}, {1, -1}, {3, -1}}
        } 
    },
    {"sqPiece", {
        "sqPiece",
        7, // Index
        1, // Number of orientations
        // Offsets from center
        {{{0, 0}, {-1, 0}, {-1, -1}, {0, -1}}},
        // Bottom Surface
        {{0}},
        // Top Surface
        {{0}},
        // Side Heights
        {{2, -2}}
        }
    }
};
