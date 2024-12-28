#version 330 core

layout(location = 0) in vec4 aRGBRadius;
layout(location = 1) in vec3 Start;
layout(location = 2) in vec3 End;

out vec3 vColor;
out vec3 vEnd;
out float vRadius;

void main()
{
    vColor = aRGBRadius.rgb;
    vRadius = aRGBRadius.a * 10;
    vEnd = End;
    gl_Position = vec4(Start, 1.0f);
}
