#ifndef TEXT
#define TEXT

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <vector>
#include <map>
#include <string>

class TextDrawer
{ 
    public:

    TextDrawer();
    std::vector<std::vector<float>> getTextVertices(std::string text, float x0, float x1, float y0, float y1);

    private:

    std::map<char, std::vector<int>> charTexCoords;
};
#endif