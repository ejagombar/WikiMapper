#version 330 core
layout(location = 0) in vec3 aCoord;
layout(location = 1) in float aWidth;

out float vWidth;

void main()
{
    gl_Position = vec4(aCoord, 1.0f);
    vWidth = aWidth;
}
