#ifndef GRID
#define GRID

#include <vector>

class Grid
{
    public: 
    
    int height, width;
    std::vector<std::vector<int>> grid;

    Grid(const int height, const int width);
    bool collisionCheck(const std::vector<std::vector<int>>& coords);
    void fillSet(const std::vector<std::vector<int>>& coords, unsigned int index);
    void fill(int row, int col, unsigned int index);
    int get(const int row, const int col);
    void clearRows(std::vector<int> filledRows);
    bool inBounds(const int row, const int col);
    std::vector<std::vector<int>> getFilledBlocks();
    std::vector<int> getFilledRows();
    void clear();
};

#endif