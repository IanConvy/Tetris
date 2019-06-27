#define GLEW_STATIC

#include "headers/text.hpp"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <vector>
#include <map>
#include <string>

TextDrawer::TextDrawer() :
charTexCoords{
    {'a', {0, 105}},
    {'b', {106, 212}},
    {'c', {213, 319}},
    {'d', {320, 426}},
    {'e', {427, 533}},
    {'f', {534, 640}},
    {'g', {641, 747}},
    {'h', {748, 867}},
    {'i', {868, 961}},
    {'j', {962, 1068}},
    {'k', {1069, 1188}},
    {'l', {1189, 1282}},
    {'m', {1283, 1389}},
    {'n', {1390, 1496}},
    {'o', {1497, 1603}},
    {'p', {1604, 1710}},
    {'q', {1711, 1817}},
    {'r', {1818, 1924}},
    {'s', {1925, 2044}},
    {'t', {2045, 2138}},
    {'u', {2139, 2245}},
    {'v', {2246, 2353}},
    {'w', {2534, 2460}},
    {'x', {2461, 2580}},
    {'y', {2581, 2673}},
    {'z', {2674, 2780}},
    {'0', {2781, 2901}},
    {'1', {2902, 2995}},
    {'2', {2996, 3103}},
    {'3', {3104, 3209}},
    {'4', {3210, 3316}},
    {'5', {3317, 3423}},
    {'6', {3424, 3530}},
    {'7', {3531, 3637}},
    {'8', {3638, 3744}},
    {'9', {3745, 3864}},
    {'-', {3865, 3960}},
    {' ', {3950, 3960}},
    {'.', {3960, 4066}}}
{}

std::vector<std::vector<float>> TextDrawer::getTextVertices(std::string text, float x0, float x1, float y0, float y1)
{
    float horiz_spacing = (x1 - x0) / text.length();
    float vert_spacing = horiz_spacing*(94.0/106.0);
    float vert_offset = (y1 - y0 - vert_spacing);
    float y0char = y0 + 0.5*vert_offset;
    float y1char = y1 - 0.5*vert_offset;
    float charCount = 0;
    std::vector<std::vector<float>> textVertices;
    for (auto& c : text) {
        float x0Char = x0 + horiz_spacing*charCount;
        float x1Char = x0Char + horiz_spacing;
        float x0Tex = charTexCoords[c][0] / 4066.0;
        float x1Tex = charTexCoords[c][1] / 4066.0;
        std::vector<float> charVertices{
            x0Char, y1char, 0,  x0Tex, 0,
            x1Char, y1char, 0,  x1Tex, 0,
            x0Char, y0char, 0,  x0Tex, 1,
            x1Char, y0char, 0,  x1Tex, 1};
        textVertices.push_back(charVertices);
        ++charCount;
    }
    return textVertices;
}
