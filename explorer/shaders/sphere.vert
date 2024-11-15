// #version 330 core
//
// layout(location = 0) in vec3 squareVertices;
// layout(location = 1) in vec4 xyzs; // Position of the center of the node and size of the square
// layout(location = 2) in vec4 color;
//
// // Output data ; will be interpolated for each fragment.
// out vec2 UV;
// out vec4 nodecolor;
//
// // Values that stay constant for the whole mesh.
// uniform vec3 CameraRight_worldspace;
// uniform vec3 CameraUp_worldspace;
// uniform mat4 VP; // Model-View-Projection matrix, but without the Model (the position is in BillboardPos; the orientation depends on the camera)
//
// void main()
// {
//     // float nodeSize = xyzs.w; // because we encoded it this way.
//     // vec3 nodeCenter_worldspace = xyzs.xyz;
//
//     float nodeSize = 1;
//     vec3 nodeCenter_worldspace = vec3(0, 0, 0);
//
//     // Calculate world position of the vertex
//     vec3 vertexPosition_worldspace = nodeCenter_worldspace
//             + CameraRight_worldspace * squareVertices.x * nodeSize
//             + CameraUp_worldspace * squareVertices.y * nodeSize;
//
//     // Output position of the vertex
//     // gl_Position = vec4(vertexPosition_worldspace, 1.0f);
//     gl_Position = vec4(nodeCenter_worldspace, 1.0f);
//
//     // Calculate UV coordinates
//     UV = squareVertices.xy + vec2(0.5, 0.5);
//
//     // Pass color to fragment shader
//     nodecolor = color;
// }
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 PV;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = PV * model * vec4(aPos, 1.0);
}
