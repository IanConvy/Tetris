#define GLEW_STATIC

#include "headers/text.hpp"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <vector>
#include <map>
#include <string>

/*
 * The TextDrawer class is used to manage the drawing of text by OpenGL. The
 * font characters are drawn using textures sampled from a bitmap, with each
 * character occupying a segment of the bitmap image. Given a set of rectangular
 * coordinates and a string of characters, the class is able to return the 
 * vertices needed for each charatcer. 
 */

TextDrawer::TextDrawer() :
/*
 * Since the font bitmap consists of a 1D sequence of characters, the position
 * of each character in the map can be specified using just a pair of integers
 * denoting the start and end of the segment containing the character. The position
 * of each character is listed in the charTexCoords map.
 */
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
    {'w', {2354, 2460}},
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
/*
 * This function takes a string of text and a set of coordinates marking the four
 * corners of a rectangle and determines the amount of space that can be allotted
 * to each character. The basic approach is to divide the horizontal length by the
 * number of character to get the width-per-character, then multiply this spacing
 * by the height/width ratio of the font to get the vertical spacing. The text is 
 * then iterated through and each character is assinged its position and texture
 * coordinates. 
 */
{
    // Get horizontal and vertical spacing
    float horiz_spacing = (x1 - x0) / text.length();
    float vert_spacing = horiz_spacing*(94.0/106.0); // 106/94 is the aspect ratio of the font
    float vert_offset = (y1 - y0 - vert_spacing); // Text is vertically centered in the rectangle
    float y0char = y0 + 0.5*vert_offset;
    float y1char = y1 - 0.5*vert_offset;

    // Iterate through each character and generate a set of four vertices to use when drawing
    float charCount = 0;
    std::vector<std::vector<float>> textVertices;
    for (auto& c : text) {
        // The x-coordinates of each character are generated from the horizontal spacing
        float x0Char = x0 + horiz_spacing*charCount;
        float x1Char = x0Char + horiz_spacing;
        // Get relative texture coordinate by dividing by total bitmap width
        float x0Tex = charTexCoords[c][0] / 4066.0; 
        float x1Tex = charTexCoords[c][1] / 4066.0;
        // Since the text is drawn in a line the y-coordinate is uniform across all characters
        std::vector<float> charVertices{
            x0Char, y1char,  x0Tex, 0,
            x1Char, y1char,  x1Tex, 0,
            x0Char, y0char,  x0Tex, 1,
            x1Char, y0char,  x1Tex, 1};
        textVertices.push_back(charVertices);
        ++charCount;
    }
    return textVertices;
}
