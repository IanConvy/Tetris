#include "headers/grid.hpp"

#include <vector>
#include <algorithm>

Grid::Grid(const int height, const int width) :
/*
 * The Grid class stores and manipulates the Tetris playfield, recording
 * piece placements and checking for collisions. It is generally encapsulated
 * in a Board instance which controls piece placement, but can exist independently.
 * The playfield is stored in a vector of vectors, where the elements of the inner
 * vectors are positive integers with meaning dictated by the game mode. 
 */
height{height},
width{width}
{
    clear(); // Generate grids
}

int Grid::get(const int row, const int col)
/*
 * This function returns the value at the specified grid position. If the row/col 
 * is negative or beyond the height/width of the grid, -1 is returned to indicate 
 * failure.
 */
{
    return inBounds(row, col) ? grid[row][col] : -1;
}

void Grid::fill(int row, int col, unsigned int index)
/*
 * This function assigns the passed index value to the specified position
 * on the grid. If the requested position is out of bounds, then the
 * function call has no effect. 
 */
{
    if (inBounds(row, col)) {
        grid[row][col] = index;
    }
}

void Grid::fillSet(const std::vector<std::vector<int>>& coords, unsigned int index)
/*
 * This function calls the fill method for a set of coordinates that all share
 * the same index. This is intended for the kind of bulk grid assingment that
 * occurs during piece placement. 
 */
{
    for (const auto& rowCol : coords) {
        fill(rowCol[0], rowCol[1], index);
    }  
}

void Grid::swapGrids()
// Slated for deletion. 
{
    grid.swap(bufferGrid);
}

void Grid::clearRows(std::vector<int> filledRows, bool buffer)
/*
 * This function deletes the rows indexed in the filledRows argument
 * and shifts the remaining rows down so that there aren't any gaps. The
 * algorithm works by going through each row and counting the number of 
 * filled rows that are lower. This determines how far down the row is going
 * to fall after the filled rows are removed. The row vectors corresponding to
 * the original position and final position are then swapped, shifting the
 * contents down. The topmost rows do not have anything above them and are 
 * therefore cleared.  
 */
{
    // Slated for removal. 
    if (buffer) {
        bufferGrid = grid;
    }
    std::sort(filledRows.begin(), filledRows.end());
    int numFilled = filledRows.size();
    for (int row = 0; row < height; ++row) {
        int fallPosition = row; // This will hold the index of the row's final position
        for (auto rowNum : filledRows) {
            if (rowNum < row) {--fallPosition;}
            if (rowNum == row) {fallPosition = row;} // If the row is filled, it's not swapped
        }
        if (fallPosition < row) {grid[row].swap(grid[fallPosition]);}
        if (row >= height - numFilled) { // Clear the topmost rows which don't have anything above them
            std::fill(grid[row].begin(), grid[row].end(), 0);
        }   
    }
    // Slated for removal
    if (buffer) {
        swapGrids();
    }
}

bool Grid::collisionCheck(const std::vector<std::vector<int>>& coords)
/*
 * This function checks to see if any of the passed coordinates, which 
 * generally represent a piece, overlap with floor, side-walls, or previous 
 * pieces. Collision with the ceiling is not checked, since in Tetris parts 
 * of a piece are usually allowed to exist above the playfiled so that the 
 * piece can be rotated immediately after entry.  
 */
{
    bool collision = false;
    for (auto& rowCol : coords) {
        bool wallCollide = rowCol[0] < 0 || rowCol[1] < 0 || rowCol[1] >= width;
        bool pieceCollide = rowCol[0] < height && get(rowCol[0], rowCol[1]);
        if (wallCollide || pieceCollide) {
            collision = true;
        }
    }
    return collision;
}

std::vector<std::vector<int>> Grid::getFilledBlocks()
/*
 * This function finds the positions and indices of all filled blocks,
 * returning a container of vector tuples giving the row, column, and 
 * index value.  
 */
{
    std::vector<std::vector<int>> filledBlocks;
    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            auto index = grid[row][col];
            if (index) { // Unfilled blocks have index 0
                filledBlocks.push_back(std::vector<int>{row, col, index});
            }
            
        }
    }
    return filledBlocks;
}

bool Grid::inBounds(const int row, const int col)
/*
 * This function checks if a given row/column pair is within
 * the boundaries of the grid (i.e. non-negative and less than
 * the height/width). Unlike the collisionCheck member, this 
 * function will check if a coordinate is above the ceiling. 
 */
{
    return row >= 0 && row < height && col >= 0 && col < width;
}

std::vector<int> Grid::getFilledRows()
/*
 * This function returns the set of indices corresponding to rows 
 * which are completely filled, meaning that all of their indices 
 * are non-zero. This function is used during play to determine
 * which rows need to be cleared. 
 */ 
{
    std::vector<int> filledRows;
    for (int row = 0; row < height; ++row) {
        auto vec = grid[row];
        if (std::all_of(vec.begin(), vec.end(), [] (int val) {return val != 0;})) {
            filledRows.push_back(row);
        }
    }
    return filledRows;
}

void Grid::clear()
/*
 * This function resets the grid by setting all elements to zero, corresponding
 * to a grid of empty blocks. 
 */ 
{
    grid = std::vector<std::vector<int>>(height, std::vector<int>(width, 0));
    bufferGrid = std::vector<std::vector<int>>(height, std::vector<int>(width, 0));
}
