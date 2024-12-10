#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in float aSize;

out vec3 vColor;
out float vSize;

void main()
{
    vColor = aColor;
    vSize = aSize;

    gl_Position = vec4(aPos, 1.0f);
}
