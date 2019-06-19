#ifndef TOOLS
#define TOOLS

#include <vector>
#include <cmath>
#include <iostream>

std::vector<int> getHeights(int height, int width, std::vector<int>& grid);
int getRoughness(int height, int width, std::vector<int>& grid);
void printGrid(int height, int width, std::vector<int>& grid);

#endif