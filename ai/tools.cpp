#include <vector>
#include <cmath>
#include <iostream>

std::vector<int> getHeights(int height, int width, std::vector<int> grid)
{
    std::vector<int> heights;
    for (int row = 0; row < height; ++row) {
        int currHeight = 0;
        for (auto itr = grid.begin() + row; *itr != 0 && currHeight < height; itr += width) {
            ++currHeight;
        }
        heights.push_back(currHeight);
    }
    return heights;
}

int getRoughness(int height, int width, std::vector<int> grid)
{
    auto heights = getHeights(height, width, grid);
    int roughness = 0;
    for (auto itr = heights.begin() + 1; itr != heights.end(); ++itr) {
        roughness += std::abs(*itr - *(itr - 1));
    }
    return roughness;
}

int main() {
    std::vector<int> test{
        1, 1, 1, 0,
        0, 1, 0, 0,
        0, 1, 0, 0,
        0, 0, 0, 0};
    std::cout << getRoughness(4, 4, test) << std::endl;
}