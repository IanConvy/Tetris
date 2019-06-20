#include "headers/player.hpp"
#include "headers/tools.hpp"

#include <iostream>
#include <vector>

int main()
{
    // std::vector<int> grid = {
    //     1, 1, 1, 0, 1,
    //     1, 1, 1, 0, 1,
    //     0, 1, 1, 0, 1,
    //     1, 0, 1, 0, 1};
    // auto lowerHeight = getLowerHeights(4, 5, grid);
    // auto upperHeight = getUpperHeights(4, 5, grid);
    // for (int col = 0; col < 5; ++col) {
    //     std::cout << lowerHeight[col] << " " << upperHeight[col] << std::endl;
    // }
    // std::cout << getHoles(lowerHeight, upperHeight, grid) << std::endl;
    Player player;
    std::vector<std::vector<int>> grids;
    player.sweepPiece(grids);
    for (int count = 0; count < 1000; ++count) {
        player.bestMove();
        printGrid(20, 10, player.grid.grid);
        std::cin.ignore();
    }
}