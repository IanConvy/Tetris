#version 330 core

layout (location = 0) in ivec3 aPos;
layout (location = 1) in ivec2 aTexCoord;

uniform int totalWidth;
uniform int totalHeight;

out vec2 TexCoord;

void main() {
    float centerX = totalWidth / 2.0;
    float centerY = totalHeight / 2.0;
    gl_Position = vec4(
        2.0 * (aPos.x - centerX) / totalWidth, 
        -2.0 * (aPos.y - centerY) / totalHeight, 
        1.0 * aPos.z, 
        1.0);
    TexCoord = 1.0 * aTexCoord;
};