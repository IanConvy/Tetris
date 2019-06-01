#include "headers/grid.hpp"

#include <vector>
#include <utility>

Grid::Grid(const int height, const int width) :
height{height},
width{width},
grid(height*width, 0)
{}

int Grid::get(const int row, const int col)
{
    return grid[width*row + col];
}

void Grid::fill(const int row, const int col, const int index)
{
    grid[width*row + col] = index;
}

bool Grid::collisionCheck(const std::vector<std::pair<int, int>> coords)
{
    bool collision = false;
    for (auto& rowCol : coords) {
        bool outOfBounds = rowCol.first < 0 || rowCol.second < 0 || rowCol.second >= width;
        if (outOfBounds || rowCol.first < height && get(rowCol.first, rowCol.second)) {
            collision = true;
        }
    }
    return collision;
}

std::vector<std::vector<int>> Grid::getFilledBlocks()
{
    std::vector<std::vector<int>> filledBlocks;
    int row = 0, col = 0;
    for (auto val : grid) {
        if (val) {
            filledBlocks.push_back(std::vector<int>{row, col, val});
        }
        ++col;
        if (col >= width) {
            col = 0;
            ++row;
        }
    }
    return filledBlocks;
}
