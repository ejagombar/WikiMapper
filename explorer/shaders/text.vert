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
uniform vec3 CameraPos;

void main()
{
    float dis = distance(CameraPos, BillboardPos);
    float size = BillboardSize;
    if (dis < 20.0)
        size *= dis * 0.05;

    vec3 vertexPosition_worldspace =
        BillboardPos
            + CameraRight_worldspace * aCoord.x * size
            + CameraUp_worldspace * aCoord.y * size
            + normalize(CameraPos - BillboardPos) * aCoord.z;

    gl_Position = projection * view * vec4(vertexPosition_worldspace, 1.0f);

    TexCoords = aTexCoords;
}
