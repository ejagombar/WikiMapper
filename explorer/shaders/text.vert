#version 330 core
layout(location = 0) in vec3 aCoord;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 BillboardPos;
uniform float BillboardSize;
uniform vec3 CameraRight_worldspace;
uniform vec3 CameraUp_worldspace;

void main()
{
    vec3 vertexPosition_worldspace =
        BillboardPos
            + CameraRight_worldspace * aCoord.x * BillboardSize
            + CameraUp_worldspace * aCoord.y * BillboardSize;

    gl_Position = projection * view * vec4(vertexPosition_worldspace, 1.0f);

    TexCoords = aTexCoords;
}
