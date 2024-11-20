#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in float aSize;

// uniform mat4 PV;
out vec3 vColor;
out float size;

void main()
{
    vColor = aColor;
    size = aSize;
    gl_Position = vec4(aPos, 1.0f);
}
