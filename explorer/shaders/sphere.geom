#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 Projection;
uniform mat4 View;

in vec3 vPos[];
in vec3 vColor[];
in float vSize[];

out vec3 fPos; // Pass world position to fragment shader
out vec3 fColor;
out vec2 mapping;

void main()
{
    fPos = vPos[0];
    fColor = vColor[0];

    // Extract world-space camera basis vectors
    vec3 cameraRight = vec3(View[0][0], View[1][0], View[2][0]);
    vec3 cameraUp = vec3(View[0][1], View[1][1], View[2][1]);

    vec3 center = vPos[0];
    vec3 offsetX = vSize[0] * cameraRight;
    vec3 offsetY = vSize[0] * cameraUp;

    mat4 PV = Projection * View;

    mapping = vec2(-1.0f, -1.0f);
    gl_Position = PV * vec4(center - offsetX - offsetY, 1.0);
    EmitVertex();

    mapping = vec2(-1.0f, 1.0f);
    gl_Position = PV * vec4(center - offsetX + offsetY, 1.0);
    EmitVertex();

    mapping = vec2(1.0f, -1.0f);
    gl_Position = PV * vec4(center + offsetX - offsetY, 1.0);
    EmitVertex();

    mapping = vec2(1.0f, 1.0f);
    gl_Position = PV * vec4(center + offsetX + offsetY, 1.0);
    EmitVertex();

    EndPrimitive();
}
