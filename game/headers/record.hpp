#ifndef RECORD
#define RECORD

#include <string>
#include <vector>

struct MoveRecord
{
    std::string pieceName;
    std::string nextPieceName;
    std::vector<int> gameGrid;
    int level;
    int lineCount;
    std::vector<int> lineTypeCount;
};

#endif