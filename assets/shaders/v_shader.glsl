#version 330 core // Set OpenGL version 3.3

/*
 * This shader takes as input the pixel position of the vertices as measured 
 * when the board size is fixed to the values given by the totalWidth and 
 * totalHeight uniforms. Since OpenGL expects positions to be based on axes that
 * are located in the center of the window and constrained to range from +1 to -1, 
 * we need to first subtract the center pixel position from the inputs and then
 * divide the height/width by two times the totalWidth/totalHeight value. We also 
 * need to flip the direction of the pixel y-axis since it normally runs down the 
 * image. The texture coordinates are sent on to the fragment shader unmodified. 
 */

layout (location = 0) in vec2 pixelPos; // Physical positions of vertices given in pixels from top-left corner
layout (location = 1) in vec2 texturePosIn; // Texture positions of vertices in range [0, 1] from bottom-left corner

uniform float totalWidth; // Total width of the board at same pixel scale as pixelPos
uniform float totalHeight; // Total height of the board at same pixel scale as pixelPos

out vec2 texturePos; // Must have a different name than the input variable

void main() {

    // Calculate position of the center in "pixel space"
    float centerX = totalWidth * 0.5;
    float centerY = totalHeight * 0.5;

    // Calculate the new coordinates on the scaled and centered OpenGL axes
    float relX = 2.0 * (pixelPos.x - centerX) / totalWidth;
    float relY = -2.0 * (pixelPos.y - centerY) / totalHeight;

    // Create outputs
    gl_Position = vec4(relX, relY, 0, 1.0);
    texturePos = texturePosIn;
};