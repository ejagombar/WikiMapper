#version 330 core

layout(location = 0) in vec4 aRGBRadius;
layout(location = 1) in vec3 aPos;

out vec3 vColor;
out float vSize;

flat out uint vNodeID;
uniform int selectedID;

void main()
{
    if (selectedID == gl_VertexID) {
        vColor = aRGBRadius.rgb * vec3(1.1);
        vSize = aRGBRadius.a * 11;
    } else {
        vColor = aRGBRadius.rgb;
        vSize = aRGBRadius.a * 10;
    }

    vNodeID = uint(gl_VertexID);
    gl_Position = vec4(aPos, 1.0f);
}
