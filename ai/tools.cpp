#include "headers/tools.hpp"

#include <vector>
#include <cmath>
#include <iostream>

std::vector<int> getHeights(int height, int width, std::vector<int>& grid)
{
    std::vector<int> heights;
    for (int col = 0; col < width; ++col) {
        int currHeight = 0;
        for (auto itr = grid.begin() + col; *itr != 0 && currHeight < height; itr += width) {
            ++currHeight;
        }
        heights.push_back(currHeight);
    }
    return heights;
}

int getRoughness(int height, int width, std::vector<int>& grid)
{
    auto heights = getHeights(height, width, grid);
    int roughness = 0;
    for (auto itr = heights.begin() + 1; itr != heights.end(); ++itr) {
        roughness += std::abs(*itr - *(itr - 1));
    }
    return roughness;
}

void printGrid(int height, int width, std::vector<int>& grid)
{
    for (int row = height - 1; row >= 0; --row) {
        auto itr = grid.begin() + width*row;
        for (int col = 0; col < width; ++col) {
            std::cout << *itr << " ";
            ++itr;
        } 
        std::cout << "\n";
    }
    std::cout << std::flush;
}