#include "headers/tools.hpp"

#include <vector>
#include <cmath>
#include <iostream>

std::vector<int> getLowerHeights(int height, int width, std::vector<int>& grid)
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

std::vector<int> getUpperHeights(int height, int width, std::vector<int>& grid)
{
    std::vector<int> heights;
    for (int col = 0; col < width; ++col) {
        int currHeight = height - 1;
        for (auto itr = grid.end() - width  + col; *itr == 0 && currHeight >= 0; itr -= width) {
            --currHeight;
        }
        heights.push_back(currHeight + 1);
    }
    return heights;
}

int getRoughness(std::vector<int>& upperHeights)
{
    int roughness = 0;
    for (auto itr = upperHeights.begin() + 1; itr != upperHeights.end(); ++itr) {
        roughness += std::abs(*itr - *(itr - 1));
    }
    return roughness;
}

int getHoles(std::vector<int>& lowerHeights, std::vector<int>& upperHeights, std::vector<int>& grid)
{
    int holes = 0;
    int width = lowerHeights.size();
    for (int col = 0; col < width; ++col) {
        auto startItr = grid.begin() + width*lowerHeights[col] + col;
        auto endItr = grid.begin() + width*upperHeights[col] + col;
        for (; startItr != endItr; startItr += width) {
            if (*startItr == 0) {
                ++holes;
            }
        }
    }
    return holes;
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