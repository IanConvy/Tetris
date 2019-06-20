#include "headers/player.hpp"
#include "headers/tools.hpp"

#include <iostream>
#include <vector>

int main()
{
    Player player;
    std::vector<std::vector<int>> grids;
    player.sweepPiece(grids);
    for (auto grid : grids) {
        printGrid(20, 10, grid);
        std::cin.ignore();
    }
}