#include "headers/player.hpp"
#include "headers/tools.hpp"

#include <iostream>

int main()
{
    Player player;
    std::cout << player.currPiece->data.index << std::endl;
    player.placePiece(4, 0);
    printGrid(20, 10, player.grid.grid);
}