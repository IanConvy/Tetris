#ifndef GRID
#define GRID

#include <vector>
#include <utility>

struct Grid
{
    const int height, width;
    std::vector<int> grid;

    Grid(const int height, const int width);
    bool collisionCheck(std::vector<std::pair<int, int>> coords);
    void fill(const int row, const int col, const int index);
    int get(const int row, const int col);
    std::vector<std::vector<int>> getFilledBlocks();
};

#endif