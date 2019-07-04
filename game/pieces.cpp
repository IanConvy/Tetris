#include "headers/pieces.hpp"

#include <vector>
#include <string>
#include <map>
#include <memory>

PieceData::PieceData(
    std::string name,
    const unsigned int pieceIndex,
    const int numOrients,
    std::vector<std::vector<std::vector<int>>> coordOffsets,
    std::vector<std::vector<int>> bottomSurf,
    std::vector<std::vector<int>> topSurf,
    std::vector<std::vector<int>> sideHeights) :
name{name}, 
index{pieceIndex},
numOrients{numOrients},
coordOffsets{coordOffsets},
bottomSurf{bottomSurf},
topSurf{topSurf},
sideHeights{sideHeights}
{}

PieceData::PieceData() :
name{},
index{0},
numOrients{1},
coordOffsets{{{{0, 0}, {0, 0}, {0, 0}, {0, 0}}}},
bottomSurf{{}},
topSurf{{}},
sideHeights{{1, -1}}
{}

Piece::Piece() :
data{nullPiece},
centerRow{0},
centerCol{0},
orient{0},
coords{{0, 0}, {0, 0}, {0, 0}, {0, 0}}
{}

Piece::Piece(const PieceData& data) :
data{data},
centerRow{0},
centerCol{0},
orient{0},
coords{{0, 0}, {0, 0}, {0, 0}, {0, 0}}
{}

void Piece::setPosition(int newCenterRow, int newCenterCol, unsigned int newOrient)
{
    centerRow = newCenterRow;
    centerCol = newCenterCol;
    orient = newOrient;
    for (int i = 0; i < data.coordOffsets[orient].size(); ++i) {
        coords[i][0] = centerRow + data.coordOffsets[orient][i][0];
        coords[i][1] = centerCol + data.coordOffsets[orient][i][1];
    }
}

void Piece::rotate(int turns)
{
    int newOrient = (data.numOrients + (orient + turns) % data.numOrients) % data.numOrients;
    setPosition(centerRow, centerCol, newOrient);
}

void Piece::translate(int dRow, int dCol)
{
    int newCenterRow = centerRow + dRow;
    int newCenterCol = centerCol + dCol;
    setPosition(newCenterRow, newCenterCol, orient);
}

PieceGenerator::PieceGenerator(std::vector<std::string> pieceList) :
uDistr(0, pieceList.size() > 0 ? pieceList.size() - 1 : 0),
pieceList{pieceList}
{
    std::random_device rDev;
    rEng.seed(rDev());
}

std::unique_ptr<Piece> PieceGenerator::getRandomPiece()
{
    int index = uDistr(rEng);
    auto pieceItr = allPieces.find(pieceList[index]);
    std::unique_ptr<Piece> randPiece{pieceItr != allPieces.end() ? new Piece(pieceItr->second) : new Piece()};
    return randPiece;
}

std::unique_ptr<Piece> PieceGenerator::getPiece(std::string pieceName)
{
    auto pieceItr = allPieces.find(pieceName);
    std::unique_ptr<Piece> piece{pieceItr != allPieces.end() ? new Piece(pieceItr->second) : new Piece()};
    return piece;
}

std::vector<std::string> PieceGenerator::getRandomSequence(int length)
{
    std::vector<std::string> sequence;
    sequence.reserve(length);
    for (int i = 0; i < length; ++i) {
        int index = uDistr(rEng);
        sequence.push_back(pieceList[index]);
    }
    return sequence;
}

const PieceData nullPiece{};

const std::map<const std::string, PieceData> allPieces{
    {"lrPiece", {
        "lrPiece",
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
    {"llPiece", {
        "llPiece",
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
    {"srPiece", {
        "srPiece",
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
    {"slPiece", {
        "slPiece",
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
