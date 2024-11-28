#version 330 core
layout(location = 0) in vec3 aCoord;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    gl_Position = projection * view * vec4(aCoord, 1.0);
    TexCoords = aTexCoords;
}
