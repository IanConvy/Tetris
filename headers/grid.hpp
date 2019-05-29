#ifndef GRID
#define GRID

#include <vector>
#include <utility>

struct Grid
{
    const int height, width;
    std::vector<std::vector<int>> grid;

    Grid(const int height, const int width);
    bool collisionCheck(std::vector<std::pair<int, int>> coords);
};

#endif