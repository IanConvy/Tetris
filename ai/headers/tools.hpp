#ifndef TOOLS
#define TOOLS

#include <vector>
#include <cmath>
#include <iostream>

std::vector<int> getLowerHeights(int height, int width, std::vector<int>& grid);
std::vector<int> getUpperHeights(int height, int width, std::vector<int>& grid);
int getRoughness(std::vector<int>& upperHeights);
void printGrid(int height, int width, std::vector<int>& grid);
int getHoles(std::vector<int>& lowerHeights, std::vector<int>& upperHeights, std::vector<int>& grid);

#endif