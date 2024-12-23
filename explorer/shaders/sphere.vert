#version 330 core

layout(location = 0) in vec4 aRGBRadius;
layout(location = 1) in vec3 aPos;

out vec3 vColor;
out float vSize;

void main()
{
    vColor = aRGBRadius.rgb;
    vSize = aRGBRadius.a * 10;

    gl_Position = vec4(aPos, 1.0f);
}
