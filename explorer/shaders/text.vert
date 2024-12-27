#version 330 core
layout(location = 0) in vec3 aCoord;
layout(location = 1) in vec2 aTexCoords;

layout(std140) uniform GlobalUniforms {
    mat4 Projection;
    mat4 View;
    vec4 CameraPosition;
};

uniform vec3 BillboardPos;
uniform float BillboardSize;
uniform vec3 CameraRight_worldspace;
uniform vec3 CameraUp_worldspace;

out vec2 TexCoords;

void main()
{
    float dis = distance(CameraPosition.xyz, BillboardPos);
    float size = BillboardSize;
    if (dis < 20.0)
        size *= dis * 0.05;
    if (dis < 1.5)
        size = 0;

    vec3 vertexPosition_worldspace =
        BillboardPos
            + CameraRight_worldspace * aCoord.x * size
            + CameraUp_worldspace * aCoord.y * size
            + normalize(CameraPosition.xyz - BillboardPos) * aCoord.z;

    gl_Position = Projection * View * vec4(vertexPosition_worldspace, 1.0f);

    TexCoords = aTexCoords;
}
