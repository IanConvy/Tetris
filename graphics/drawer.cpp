#include "headers/drawer.hpp"

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "headers/stb_image.hpp"
#include "headers/shader.hpp"
#include "headers/text.hpp"
#include "../game/headers/pieces.hpp"
#include "../game/headers/grid.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <map>
#include <sstream>
#include <iomanip>

/*
 * The BoardDrawer class encapsulates all of the calls to OpenGL that are
 * necessary to display the game graphics. The drawer works independently 
 * from the game engine itself, with their only connection being a set of 
 * poiners that indicate which data the drawer should use when drawing
 * a frame.  
 */

BoardDrawer::BoardDrawer(std::string location) : 
brdShader( // Initialize the Shader instance that holds the shader program
    (location + std::string("/shaders/v_shader.glsl")).c_str(), 
    (location + std::string("/shaders/f_shader.glsl")).c_str()),
textDrawer{},
brdVertices{ // Holds the pixel positions and texture coordinates of the entire game board 
    //   Position         Texture
        0,      0,         0, 1,
        1035,   0,         1, 1,
        0,      899,       0, 0,
        1035,   899,       1, 0},
playFieldPos{ // Holds the pixel positions of the playfield
    390, 159,   710, 159,
    390, 805,   710, 805},
previewPos{ // Holds the pixel positions of the next-piece preview window
    776, 416,   902, 416,
    776, 550,   902, 550},
gridHeight{20}, // Height of the playfield grid
gridWidth{10}, // Width of the playfield grid  
blockTextures(5, 0), // Holds the texture IDs for the different types of blocks
pieceTexMap{0, 0, 1, 1, 0, 2, 2, 2, 3, 4}, // Maps the piece index to its texture
blockWidthSpacing{(playFieldPos[2] - playFieldPos[0])/gridWidth}, // Pixel width of each grid block
blockHeightSpacing{(playFieldPos[5] - playFieldPos[1])/gridHeight}, // Pixel height of each grid block
nextPieceSource{nullptr}, // Pointer to the next piece
gridSource{nullptr}, // Pointer to the grid to be displayed
lineCountSource{nullptr}, // Pointer to the line count data
scoreSource{nullptr}, // Pointer to the score data
levelSource{nullptr}, // Pointer to the level data
lineTypeCountSource{nullptr}, // Pointer to line type data
miscDataSource{nullptr}, // Pointer to the miscellaneous data
sqrArray{0}, // Holds the ID of the vertex array object
brdTexture{0}, // Holds the ID of the NES board texture
fontTexture{0}, // Holds the ID of the font bitmap texture
sqrBuffer{0}, // Holds the ID of the vertex buffer object
sqrIndexBuffer{0} // Holds the ID of the element buffer object
{   

    // Tell the shader program how big the game board is
    brdShader.setFloat("totalWidth", 1035);
    brdShader.setFloat("totalHeight", 899);

    /*
     * The most important part of this OpenGL pipeline is the vertex array object,
     * which manages the position and texture attributes of the vertices and sets
     * the order in which the vertices should be drawn to form a square. All of 
     * the objects that are drawn are drawn as squares made from two triangle
     * primitives. The array consists of a vertex buffer object to hold the data,
     * a set of attribute pointers to parse the data, and an element buffer object
     * to draw the four vertices in the correct order to from the triangle pair. 
     */

    glGenVertexArrays(1, &sqrArray);
    glBindVertexArray(sqrArray);

    glGenBuffers(1, &sqrBuffer); // Stores vertex data
    glBindBuffer(GL_ARRAY_BUFFER, sqrBuffer);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); // Pixel Position
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float))); // Texture coordinates
    glEnableVertexAttribArray(0); // Sets location = 0 to the pixle position in the shader code
    glEnableVertexAttribArray(1); // Sets location = 1 to the texture coordinates in the shader code
    
    std::vector<unsigned int> sqrIndices = { // Maps four source vertices to the six triangle vertices needed to draw a square
        0, 1, 2, // Vertices of triangle 1
        1, 2, 3};  // Vertices of triangle 2
    glGenBuffers(1, &sqrIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sqrIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sqrIndices.size() * sizeof(unsigned int), &sqrIndices[0], GL_STATIC_DRAW);
    
    // Create the textures associated with the game board, font, and blocks
    createTexture(brdTexture, location + std::string("/images/tetrisboard.png"));
    createTexture(fontTexture, location + std::string("/images/fontbitmap.png"));
    createTexture(blockTextures[0], location + std::string("/images/yellowblock.png"));
    createTexture(blockTextures[1], location + std::string("/images/redblock.png"));
    createTexture(blockTextures[2], location + std::string("/images/whiteblock.png"));
    createTexture(blockTextures[3], location + std::string("/images/allowedblock.png"));
    createTexture(blockTextures[4], location + std::string("/images/disallowedblock.png"));
}

BoardDrawer::~BoardDrawer()
/*
 * The class destructor tells OpenGL to free up all memory associated with the 
 * textures and buffer objects that were created.
 */
{
    glDeleteTextures(1, &brdTexture);
    glDeleteTextures(1, &fontTexture);
    glDeleteTextures(blockTextures.size(), &blockTextures[0]);
    glDeleteBuffers(1, &sqrBuffer);
    glDeleteBuffers(1, &sqrIndexBuffer);
    glDeleteVertexArrays(1, &sqrArray);
}

void BoardDrawer::drawFrame()
/*
 * This function is simply a convenient way of calling all of the
 * member functions that draw each part of the game. The order of
 * the function calls is arbitrary except that the board must be
 * drawn first.   
 */
{
    drawBoard(); // Must be called first
    drawPieceBlocks();
    drawPreview();
    drawLineCount();
    drawLineTypeCount();
    drawScore();
    drawLevel();
    drawMiscData();
}

void BoardDrawer::drawBoard()
/*
 * This function draws the game board, which is simply a square with vertices
 * located at the corners of the game window and sampled from the four corners
 * of the NES board texture image. 
 */ 
{
    drawSquare(brdVertices, brdTexture);
}

void BoardDrawer::drawPreview()
/*
 * This functions draws the piece preview which allows the player to see which
 * piece is coming next. The main difficulty comes from using the coordinate
 * offsets, which describe the shape of the piece, to draw a visual representation 
 * of the piece that is centered in the preview window. The steps to do this are 
 * described below.
 */
{
    if (nextPieceSource) {
        const PieceData& data  = (*nextPieceSource)->data;
        int texture = blockTextures[pieceTexMap[data.index]];

        /*
         * The first step is to find the height and width of the 
         * piece in terms of grid blocks. This is solved by finding
         * the most negative offset and most positive offset for both
         * the x and y coordinates.
         */
        int minHeight = data.coordOffsets[0][0][0];  
        int maxHeight = data.coordOffsets[0][0][0];
        int minWidth = data.coordOffsets[0][0][1]; 
        int maxWidth = data.coordOffsets[0][0][1];  

        for (auto& rowCol : data.coordOffsets[0]) {
            if (rowCol[0] < minHeight) {
                minHeight = rowCol[0];
            }
            if (rowCol[0] > maxHeight) {
                maxHeight = rowCol[0];
            }
            if (rowCol[1] < minWidth) {
                minWidth = rowCol[1];
            }
            if (rowCol[1] > maxWidth) {
                maxWidth = rowCol[1];
            }
        }

        /*
         * Next, we find the pixel length of the square blocks that will be used to draw
         * the piece by assuming that four blocks will fit in the width of the preview
         * box. 
         */
        float spacing = (previewPos[2] - previewPos[0]) / 4;

        /*
         * The most complicated step is finding the proper coordinate shift such that
         * the piece is drawn in the center of the preview box. This is done by calculating
         * the height/width of a box inscribing the piece, then adding half of that height/width
         * to the center pixel position of the preview box. The resulting coordinates mark the
         * location of the bottom left corner of the inscribing box when it is centered in the
         * preview box. Finally, the distance between the bottom left corner of the piece and
         * the bottom left corner of the centered inscribing box is calculated.
         */
        float pieceHeight = spacing * (maxHeight - minHeight + 1); 
        float pieceWidth = spacing * (maxWidth - minWidth + 1);
        float inscrBoxCornerHeight = (previewPos[5] + previewPos[1] + pieceHeight) / 2; 
        float inscrBoxCornerWidth = (previewPos[2] + previewPos[0] - pieceWidth) / 2;
        float pieceCornerHeight = -spacing*minHeight;
        float pieceCornerWidth = spacing*minWidth;
        float heightOffset = inscrBoxCornerHeight - pieceCornerHeight;
        float widthOffset = inscrBoxCornerWidth - pieceCornerWidth;

        /*
         * With the spacing, which converts the grid coordinates to pixel coordinates, and
         * the the offsets needed to center the piece, the vertices are easily created by
         * iterating through the coordOffsets and applying the affine transformation. The
         * blocks are then drawn individually. 
         */
        for (auto& rowCol : data.coordOffsets[0]) {
            const float x0 = rowCol[1]*spacing + widthOffset;
            const float x1 = (rowCol[1] + 1)*spacing + widthOffset;
            const float y0 = -rowCol[0]*spacing + heightOffset;
            const float y1 = -(rowCol[0] + 1)*spacing + heightOffset;
            std::vector<float> vertices = {
            x0, y1,      0, 1,
            x1, y1,      1, 1,
            x0, y0,      0, 0,
            x1, y0,      1, 0};
            drawSquare(vertices, texture);
        }
    }
}

void BoardDrawer::drawPieceBlocks() {
    /*
     * This function draws the blocks in the playfield that are filled. It does
     * this by getting the filled blocks from the target Grid and calculatng 
     * their pixel positions by starting from the bottom left corner of the
     * playfield and adding the correct amount of spacing based on the grid
     * coordinates. 
     */
    if (gridSource) {
        for (auto& rowColIndex : gridSource->getFilledBlocks()) {
            int row = rowColIndex[0];
            int col = rowColIndex[1];
            int index = rowColIndex[2];
            int texture = blockTextures[pieceTexMap[index]];
            const float x0 = playFieldPos[0] + col*blockWidthSpacing;
            const float x1 = playFieldPos[0] + (col + 1)*blockWidthSpacing;
            const float y0 = playFieldPos[5] - row*blockHeightSpacing;
            const float y1 = playFieldPos[5] - (row + 1)*blockHeightSpacing;
            std::vector<float> vertices = {
            x0, y1,      0, 1,
            x1, y1,      1, 1,
            x0, y0,      0, 0,
            x1, y0,      1, 0};
            drawSquare(vertices, texture);
        }
    }
}

void BoardDrawer::drawLineCount()
/*
 * This function draws the line count of the game, formatting it as
 * "LINES-XXX" where at least three digit is always displayed. 
 */
{
    if (lineCountSource) {
        int x0 = 408, x1 = 695;
        int y0 = 64, y1 = 95;
        std::string lineCountRaw = std::to_string(*lineCountSource);
        std::string lineCountStr = std::string("lines-") + 
            (lineCountRaw.size() < 3 ? std::string(3 - lineCountRaw.size(), '0') + lineCountRaw : lineCountRaw);
        auto textVertices = textDrawer.getTextVertices(lineCountStr, x0, x1, y0, y1);
        for (auto& charVertices : textVertices) {
            drawSquare(charVertices, fontTexture);
        }
    }
}

void BoardDrawer::drawLineTypeCount()
/*
 * This function draws the counters for the different types of line clears,
 * displaying at least three digits in each counter. The different counters
 * are stacked on top of each other vertically, the the x-coordinates are
 * the same 
 */
{
    if (lineTypeCountSource) {
        int x0 = 68, x1 = 333;
        int yStart = 630; // Height from which to start drawing the stack of counters
        int type = 0; // Keeps track of the line clear type during drawing loop
        std::vector<std::string> typeLabels{"single - ", "double - ", "triple - ", "tetris - "};
        for (const auto& typeCount : *lineTypeCountSource) {
            int y0 = yStart + 50*type; // Starting height changes along the stack
            int y1 = yStart + 50*type + 18; // The text is 18 pixels tall
            std::string countRaw = std::to_string(typeCount);
            std::string countStr = typeLabels[type] + 
                (countRaw.size() < 3 ? std::string(3 - countRaw.size(), '0') + countRaw : countRaw);
            auto textVertices = textDrawer.getTextVertices(countStr, x0, x1, y0, y1);
            for (auto& charVertices : textVertices) { // Draw each character of the string
                drawSquare(charVertices, fontTexture);
            }
            ++type;
        }
    }
}

void BoardDrawer::drawScore()
/*
 * This function draws the score of the game. It always displays at least eight 
 * digits.
 */
{
    if (scoreSource) {
        int x0 = 774, x1 = 980;
        int y0 = 258, y1 = 286;
        std::string scoreRaw = std::to_string(*scoreSource);
        std::string scoreStr = scoreRaw.size() < 6 ? std::string(6 - scoreRaw.size(), '0') + scoreRaw : scoreRaw;
        auto textVertices = textDrawer.getTextVertices(scoreStr, x0, x1, y0, y1);
        for (auto& charVertices : textVertices) {
            drawSquare(charVertices, fontTexture);
        }
    }
}

void BoardDrawer::drawLevel()
/*
 * This function draws the level counter of the game. It always displays at
 * least two digits.
 */
{
    if (levelSource) {
        int x0 = 843, x1 = 902;
        int y0 = 642, y1 = 671;
        std::string levelRaw = std::to_string(*levelSource);
        std::string levelStr = levelRaw.size() < 2 ? std::string(2 - levelRaw.size(), '0') + levelRaw : levelRaw;
        auto textVertices = textDrawer.getTextVertices(levelStr, x0, x1, y0, y1);
        for (auto& charVertices : textVertices) {
            drawSquare(charVertices, fontTexture);
        }
    }
}

void BoardDrawer::drawMiscData()
/*
 * This function draws a set of miscellaneous data from the game. The data
 * is organized in a vertical stack, so the x-coordinates are identical. 
 */
{
    if (miscDataSource && !miscDataSource->empty()) {
        int x0 = 68, x1 = 333;
        int yStart = 255; // Height from which to start drawing the stack of counters
        int maxHeight = 315; // Maximum amount of vertical space available
        int dataCount = 0; // Keeps track of the line clear type during drawing loop
        // The spacing between the data text is at most 50, but must otherwise be shrunk to fit within maxHeight
        float spacing = (maxHeight / miscDataSource->size() < 50) ? maxHeight / miscDataSource->size() : 50;
        for (const auto& labelValue : *miscDataSource) {
            int y0 = yStart + spacing*dataCount; // Starting height changes along the stack
            int y1 = yStart + spacing*dataCount + 22; // The text is 22 pixels tall
            std::stringstream valStream; // Set the precision of the numerical value to be displayed using a stringstream 
            valStream << std::fixed << std::setprecision(2) << labelValue.second;
            std::string valueRaw = valStream.str();
            std::string dataStr = labelValue.first + std::string("   ") + valueRaw;
            auto textVertices = textDrawer.getTextVertices(dataStr, x0, x1, y0, y1);
            for (auto& charVertices : textVertices) {
                drawSquare(charVertices, fontTexture);
            }
            ++dataCount;
        }
    }
}

void BoardDrawer::assignGrid(Grid& grid)
// Assign source of grid data
{
    gridSource = &grid;
}

void BoardDrawer::assignLevel(int& level)
// Assign source of level data
{
    levelSource = &level;
}

void BoardDrawer::assignLineCount(int& lineCount)
// Assign source of line count data
{
    lineCountSource = &lineCount;
}

void BoardDrawer::assignlineTypeCount(std::vector<int>& typecounts)
// Assign source of line type data
{
    lineTypeCountSource = &typecounts;
}

void BoardDrawer::assignNextPiece(std::unique_ptr<Piece>& piecePtr)
// Assign source of piece preview data
{
    nextPieceSource = &piecePtr;
}

void BoardDrawer::assignScore(int& score)
// Assign source of score data
{
    scoreSource = &score;
}

void BoardDrawer::assignMiscData(std::map<std::string, float>& data)
// Assign source of miscellaneous data
{
    miscDataSource = &data;
}

void BoardDrawer::drawSquare(const std::vector<float>& vertices, unsigned int texture)
/*
 * This function is the one that does all of the actual drawing, since
 * all of the primitives that need to be drawn are squares. To the process
 * simply involes binding the passed texture, binding and loading the vertex
 * array buffer, attaching the shader program, and then drawing the square
 * as a pair of triangles. 
 */
{
    glBindTexture(GL_TEXTURE_2D, texture);
    brdShader.use();
    glBindVertexArray(sqrArray);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void createTexture(unsigned int& texID, std::string filePath)
/*
 * This function is used to create an OpenGL texture from an image file saved
 * to disk. After generating a new texture object and setting the interpolation
 * settings to avoid using a mipmap, the image is loaded using thr stb_image 
 * library and then imported into the texture object.  
 */
{
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST doesn't require a mipmap
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // GL_NEAREST doesn't require a mipmap
    
    stbi_set_flip_vertically_on_load(true); // Image will be loaded upside down by default
    int width = 0, height = 0, nrChannels = 0; // Values will be set by the loader based on image file
    unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
    if (data) { // Use RGBA for PNG images
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    else {
        std::cout << "Failed to load texture." << std::endl;
    }
    stbi_image_free(data); // Delete image data    
}