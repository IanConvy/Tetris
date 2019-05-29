#ifndef PIECES
#define PIECES
#include <vector>
#include <utility>

struct PieceData
{
    const unsigned int index;
    const int numOrients;
    std::vector<std::vector<std::pair<int, int>>> coordOffsets;

    PieceData(
        const unsigned int index,
        const int numOrients,
        std::vector<std::vector<std::pair<int, int>>> coordOffsets
    );
};

struct Piece
{
    const PieceData& data;
    int centerRow, centerCol, orient;
    std::vector<std::pair<int, int>> coords;

    Piece(const PieceData& data);
    void setPosition(int centerRow, int centerCol, unsigned int orient);
    void rotate(int turns);
    void translate(int dRow, int dCol);
};

extern const PieceData lrPiece;
extern const PieceData llPiece;
extern const PieceData srPiece;
extern const PieceData slPiece;
extern const PieceData tPiece;
extern const PieceData iPiece;
extern const PieceData sqPiece;

#endif
