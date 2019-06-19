#include "headers/grid.hpp"

#include <vector>

Grid::Grid(const int height, const int width) :
height{height},
width{width},
grid(height*width, 0),
bufferGrid(height*width, 0)
{}

int Grid::get(const int row, const int col)
{
    return grid[width*row + col];
}

void Grid::fillSet(std::vector<std::vector<int>>& coords, unsigned int index)
{
    for (auto& rowCol : coords) {
        if (inBounds(rowCol[0], rowCol[1])) {
            grid[width*rowCol[0] + rowCol[1]] = index;
        }
    }  
}

void Grid::clearSet(std::vector<std::vector<int>>& coords)
{
    for (auto& rowCol : coords) {
        if (inBounds(rowCol[0], rowCol[1])) {
            grid[width*rowCol[0] + rowCol[1]] = 0;
        }
    }
}

void Grid::fill(int row, int col, unsigned int index)
{
    if (inBounds(row, col)) {
        grid[width*row + col] = index;
    }
}

void Grid::swapGrids()
{
    grid.swap(bufferGrid);
}

void Grid::clearRows(const std::vector<int>& filledRows, bool buffer)
{
    if (buffer) {
        bufferGrid = grid;
    }
    for (int i = 0; i < filledRows.size(); ++i) {
        auto endItr = grid.begin();
        auto startItr = grid.begin() + width*(filledRows[i] + 1);
        if (filledRows[i] != filledRows.back()) {
            endItr += width*filledRows[i + 1];
        }
        else {
            endItr = grid.end();
        }
        for (; startItr < endItr; ++startItr) {
            *(startItr - width*(i + 1)) = *startItr;
        }
    }
    for (auto itr = grid.end() - 1; itr > grid.end() - 1 - width*filledRows.size(); --itr) {
        *itr = 0;
    }
    if (buffer) {
        swapGrids();
    }
    
}

bool Grid::collisionCheck(const std::vector<std::vector<int>> coords)
{
    bool collision = false;
    for (auto& rowCol : coords) {
        bool outOfBounds = rowCol[0] < 0 || rowCol[1] < 0 || rowCol[1] >= width;
        if (outOfBounds || rowCol[0] < height && get(rowCol[0], rowCol[1])) {
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

bool Grid::inBounds(const int row, const int col)
{
    return row >= 0 && row < height && col >= 0 && col < width;
}

std::vector<int> Grid::getFilledRows()
{
    std::vector<int> filledRows;
    int row = 0, col = 0, colCount = 0;
    for (auto& val : grid) {
        ++col;
        if (val) {
            ++colCount;
        }
        if (col >= width) {
            if (colCount == width) {
                filledRows.push_back(row);
            }
            ++row;
            col = 0;
            colCount = 0;
        }
    }
    return filledRows;
}

void Grid::reset() 
{
    grid = std::vector<int>(height*width, 0);
    bufferGrid = std::vector<int>(height*width, 0);
}
