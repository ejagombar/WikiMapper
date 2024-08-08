#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 squareVertices;
layout(location = 1) in vec4 xyzs; // Position of the center of the node and size of the square
layout(location = 2) in vec4 color; // Position of the center of the node and size of the square

// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec4 nodecolor;
out vec3 Normal; // Normal vector for lighting calculation
out vec3 WorldPos; // World position for lighting calculation

// Values that stay constant for the whole mesh.
uniform vec3 CameraRight_worldspace;
uniform vec3 CameraUp_worldspace;
uniform mat4 VP; // Model-View-Projection matrix, but without the Model (the position is in BillboardPos; the orientation depends on the camera)

void main()
{
    float nodeSize = xyzs.w; // because we encoded it this way.
    vec3 nodeCenter_worldspace = xyzs.xyz;

    // Calculate world position of the vertex
    vec3 vertexPosition_worldspace =
        nodeCenter_worldspace
            + CameraRight_worldspace * squareVertices.x * nodeSize
            + CameraUp_worldspace * squareVertices.y * nodeSize;

    // Output position of the vertex
    gl_Position = VP * vec4(vertexPosition_worldspace, 1.0f);

    // Pass world position to fragment shader
    WorldPos = vertexPosition_worldspace;

    // Calculate UV coordinates
    UV = squareVertices.xy + vec2(0.5, 0.5);

    // Pass color to fragment shader
    nodecolor = color;

    // Calculate fake normal for lighting (points outwards from the center)
    Normal = normalize(vec3(squareVertices.xy, sqrt(1.0 - dot(squareVertices.xy, squareVertices.xy))));
}
