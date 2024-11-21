#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 Projection;
uniform mat4 View;

in vec3 vPos[];
in vec3 vColor[];
in float vSize[];

out vec3 sphereWorldPos; // Pass world position to fragment shader
out vec3 fColor;
out vec2 mapping;
out mat3 billboardToWorld; // Matrix to align normals to world space

void main()
{
    sphereWorldPos = vPos[0];
    fColor = vColor[0];

    // Extract world-space camera basis vectors
    vec3 cameraRight = vec3(View[0][0], View[1][0], View[2][0]); // First column of View matrix
    vec3 cameraUp = vec3(View[0][1], View[1][1], View[2][1]); // Second column of View matrix
    vec3 cameraForward = normalize(-vec3(View[0][2], View[1][2], View[2][2])); // Negative Z (forward direction)

    // Pass a matrix to convert local space to world space
    billboardToWorld = mat3(cameraRight, cameraUp, cameraForward);

    vec3 center = vPos[0];
    vec3 offsetX = vSize[0] * cameraRight; // Offset along the right vector
    vec3 offsetY = vSize[0] * cameraUp; // Offset along the up vector

    mat4 PV = Projection * View;

    mapping = vec2(1.0f, 1.0f);
    gl_Position = PV * vec4(center + offsetX + offsetY, 1.0);
    EmitVertex();

    mapping = vec2(-1.0f, 1.0f);
    gl_Position = PV * vec4(center - offsetX + offsetY, 1.0);
    EmitVertex();

    mapping = vec2(1.0f, -1.0f);
    gl_Position = PV * vec4(center + offsetX - offsetY, 1.0);
    EmitVertex();

    mapping = vec2(-1.0f, -1.0f);
    gl_Position = PV * vec4(center - offsetX - offsetY, 1.0);
    EmitVertex();

    EndPrimitive();
}
