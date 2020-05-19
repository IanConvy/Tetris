#version 330 core // Set OpenGL version 3.3

/*
 * This shader takes as input the texture coordinates for a vertex
 * and then maps it to the bound texture to produce the desired 
 * fragment colors. 
 */

in vec2 texturePos; // x and y coordinates in the range [0, 1]

out vec4 FragColor; // Color of the fragment extracted from the texture

uniform sampler2D ourTexture; // Texture variable corresponding to the bound texture

void main() {
    // The built-in texture function does all of the complicated sampling for us
    FragColor = texture(ourTexture, texturePos);
};