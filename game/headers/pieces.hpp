#ifndef PIECES
#define PIECES
#include <vector>
#include <string>
#include <map>
#include <random>
#include <memory>

struct PieceData
{
    std::string name;
    const unsigned int index;
    const int numOrients;
    std::vector<std::vector<std::vector<int>>> coordOffsets;
    std::vector<std::vector<int>> bottomSurf, topSurf, sideHeights;

    PieceData();
    PieceData(
        std::string name,
        const unsigned int index,
        const int numOrients,
        std::vector<std::vector<std::vector<int>>> coordOffsets,
        std::vector<std::vector<int>> bottomSurf,
        std::vector<std::vector<int>> topSurf,
        std::vector<std::vector<int>> sideHeights
    );
};

class Piece
{
    public:
    
    const PieceData& data;
    int centerRow, centerCol, orient;
    std::vector<std::vector<int>> coords;

    Piece();
    Piece(const PieceData& data);
    void setPosition(int centerRow, int centerCol, unsigned int orient);
    void rotate(int turns);
    void translate(int dRow, int dCol);
};

struct PieceGenerator
{
    std::default_random_engine rEng;
    std::uniform_int_distribution<int> uDistr;
    std::vector<std::string> pieceList;

    PieceGenerator(std::vector<std::string> pieceList);
    std::unique_ptr<Piece> getRandomPiece();
    std::unique_ptr<Piece> getPiece(std::string pieceName);
    std::vector<std::string> getRandomSequence(int length);
};

extern const std::map<const std::string, PieceData> allPieces;
extern const PieceData nullPiece;

#endif
