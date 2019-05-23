#define GLEW_STATIC
#ifndef BOARD_DRAWER
#define BOARD_DRAWER

#include "shader_s.hpp"

struct BoardDrawer
{
    unsigned int brdArray, brdTexture;
    Shader brdShader;
    BoardDrawer();
    void drawBoard();
};

#endif 