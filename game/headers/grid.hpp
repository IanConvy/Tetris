#ifndef GRID
#define GRID

#include <vector>

struct Grid
{
    const int height, width;
    std::vector<int> grid, bufferGrid;

    Grid(const int height, const int width);
    bool collisionCheck(std::vector<std::vector<int>> coords);
    void fillSet(std::vector<std::vector<int>>& coords, unsigned int index);
    void clearSet(std::vector<std::vector<int>>& coords);
    void fill(int row, int col, unsigned int index);
    int get(const int row, const int col);
    void clearRows(const std::vector<int>& filledRows, bool buffer = false);
    void swapGrids();
    bool inBounds(const int row, const int col);
    std::vector<std::vector<int>> getFilledBlocks();
    std::vector<int> getFilledRows();
    void reset();
};

#endif