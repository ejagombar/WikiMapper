#version 330 core
layout(location = 0) in vec3 aCoord;
layout(location = 1) in float aWidth;
layout(location = 2) in float aTexIndex;

out float vWidth;
flat out float vTexIndex;

void main()
{
    gl_Position = vec4(aCoord, 1.0);
    vWidth = aWidth;
    vTexIndex = aTexIndex;
}
