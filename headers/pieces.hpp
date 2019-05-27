#ifndef PIECES
#define PIECES
#include <vector>

struct PieceData
{
    const unsigned int index;
    const int numOrients;
    std::vector<std::vector<int>> rowOffsets, colOffsets;

    PieceData(
        const unsigned int index,
        const int numOrients,
        std::vector<std::vector<int>> rowOffsets, 
        std::vector<std::vector<int>> colOffsets
    );
};

struct Piece
{
    const PieceData& data;
    int centerRow, centerCol, orient;
    std::vector<int> rows, cols;

    Piece(const PieceData& data);
    void setPosition(int centerRow, int centerCol, unsigned int orient);
    void rotate(int turns);
};

extern const PieceData lrPiece;
extern const PieceData llPiece;
extern const PieceData srPiece;
extern const PieceData slPiece;
extern const PieceData tPiece;
extern const PieceData iPiece;
extern const PieceData sqPiece;

#endif
