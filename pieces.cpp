#include "headers/pieces.hpp"

#include <vector>
#include <cassert>
#include <iostream>

PieceData::PieceData(
    const unsigned int pieceIndex,
    const int numOrients,
    std::vector<std::vector<int>> rowOffsets, 
    std::vector<std::vector<int>> colOffsets) : 
index{pieceIndex},
numOrients{numOrients},
rowOffsets{rowOffsets},
colOffsets{colOffsets}
{}

Piece::Piece(const PieceData& data) :
data{data},
centerRow{0},
centerCol{0},
orient{0},
rows{0, 0, 0, 0},
cols{0, 0, 0, 0}
{}

void Piece::setPosition(int newCenterRow, int newCenterCol, unsigned int newOrient)
{
    centerRow = newCenterRow;
    centerCol = newCenterCol;
    orient = newOrient;
    assert(orient < data.numOrients);
    assert(data.rowOffsets.size() == data.colOffsets.size());
    for (int i = 0; i < data.rowOffsets[orient].size(); ++i) {
        rows[i] = centerRow + data.rowOffsets[orient][i];
        cols[i] = centerCol + data.colOffsets[orient][i];
    }
}

void Piece::rotate(int turns)
{
    int newOrient = (data.numOrients + (orient + turns) % data.numOrients) % data.numOrients;
    assert(newOrient >= 0 && newOrient < data.numOrients);
    setPosition(centerRow, centerCol, newOrient);
}

void Piece::translate(int dRow, int dCol)
{
    int newCenterRow = centerRow + dRow;
    int newCenterCol = centerCol + dCol;
    setPosition(newCenterRow, newCenterCol, orient);
}

const PieceData lrPiece{
    0, // Index
    4, // Number of orientations
    // Row offsets from center
    {{0, 0, 0, -1},
    {0, -1, 1, 1},
    {0, 0, 0, 1},
    {0, 1, -1, -1}},
    // Column offsets from center
    {{0, 1, -1, -1},
    {0, 0, 0, -1},
    {0, -1, 1, 1},
    {0, 0, 0, 1}}
};

const PieceData llPiece{
    1, // Index
    4, // Number of orientations
    // Row offsets from center
    {{0, 0, 0, -1},
    {0, 1, -1, -1},
    {0, 0, 0, 1},
    {0, -1, 1, 1}},
    // Column offsets from center
    {{0, -1, 1, 1},
    {0, 0, 0, -1},
    {0, 1, -1, -1},
    {0, 0, 0, 1}}         
};

const PieceData srPiece{
    2, // Index
    2, // Number of orientations
    // Row offsets from center
    {{0, 0, -1, -1},
    {0, 1, 0, -1}},
    // Column offsets from center
    {{0, 1, -1, 0},
    {0, 0, 1, 1}}
};

const PieceData slPiece{
    3, // Index
    2, // Number of orientations
    // Row offsets from center
    {{0, 0, -1, -1},
    {0, -1, 0, 1}},
    // Column offsets from center
    {{0, -1, 0, 1},
    {0, 0, 1, 1}}
};

const PieceData iPiece{
    4, // Index
    2, // Number of orientations
    // Row offsets from center
    {{0, 0, 0, 0},
    {0, 2, 1, -1}},
    // Column offsets from center
    {{0, -2, -1, 1},
    {0, 0, 0, 0}}
};

const PieceData tPiece{
    5, // Index
    4, // Number of orientations
    // Row offsets from center
    {{0, -1, 0, 0},
    {0, 0, -1, 1},
    {0, 1, 0, 0},
    {0, 0, 1, -1}},
    // Column offsets from center
    {{0, 0, 1, -1},
    {0, -1, 0, 0},
    {0, 0, -1, 1},
    {0, 1, 0, 0}}
};

const PieceData sqPiece{
    6, // Index
    1, // Number of orientations
    // Row offsets from center
    {{0, -1, -1, 0}},
    // Column offsets from center
    {{0, 0, -1, -1}}
};
