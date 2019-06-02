#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform float totalWidth;
uniform float totalHeight;

out vec2 TexCoord;

void main() {
    float centerX = totalWidth / 2.0;
    float centerY = totalHeight / 2.0;
    gl_Position = vec4(
        2.0 * (aPos.x - centerX) / totalWidth, 
        -2.0 * (aPos.y - centerY) / totalHeight, 
        aPos.z, 
        1.0);
    TexCoord = aTexCoord;
};