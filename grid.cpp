#include "headers/grid.hpp"

#include <vector>
#include <utility>

Grid::Grid(const int height, const int width) :
height{height},
width{width},
grid(height, std::vector<int>(width, 0))
{}

bool Grid::collisionCheck(const std::vector<std::pair<int, int>> coords)
{
    bool collision = false;
    for (auto& rowCol : coords) {
        bool outOfBounds = rowCol.first < 0 || rowCol.second < 0 || rowCol.second >= width;
        if (outOfBounds || rowCol.first < height && grid[rowCol.first][rowCol.second]) {
            collision = true;
        }
    }
    return collision;
}
