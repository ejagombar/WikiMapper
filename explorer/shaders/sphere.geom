#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform float time;

layout(std140) uniform GlobalUniforms {
    mat4 projection;
    mat4 view;
    vec4 cameraPosition;
};

in vec3 vColor[];
in float vSize[];

out vec3 fPos; // Pass world position to fragment shader
out vec3 fColor;
out float fSize;
out vec2 mapping;

const float g_boxCorrection = 1.5;

void main() {
    fPos = gl_in[0].gl_Position.xyz;
    fColor = vColor[0];
    fSize = vSize[0];

    // Extract world-space camera basis vectors
    vec3 cameraRight = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 cameraUp = vec3(view[0][1], view[1][1], view[2][1]);

    vec3 position = gl_in[0].gl_Position.xyz;
    float scale = 1;

    vec3 center = gl_in[0].gl_Position.xyz + vec3(sin(time + gl_in[0].gl_Position.z * 0.1) * 0.04, sin(time + gl_in[0].gl_Position.y * 0.1) * 0.055, sin(time + gl_in[0].gl_Position.x * 0.1) * 0.07);

    vec3 offsetX = vSize[0] * cameraRight;
    vec3 offsetY = vSize[0] * cameraUp;

    mat4 PV = projection * view;

    mapping = vec2(-1.0f, -1.0f) * g_boxCorrection;
    gl_Position = PV * vec4(center - offsetX - offsetY, 1.0);
    EmitVertex();

    mapping = vec2(-1.0f, 1.0f) * g_boxCorrection;
    gl_Position = PV * vec4(center - offsetX + offsetY, 1.0);
    EmitVertex();

    mapping = vec2(1.0f, -1.0f) * g_boxCorrection;
    gl_Position = PV * vec4(center + offsetX - offsetY, 1.0);
    EmitVertex();

    mapping = vec2(1.0f, 1.0f) * g_boxCorrection;
    gl_Position = PV * vec4(center + offsetX + offsetY, 1.0);
    EmitVertex();

    EndPrimitive();
}
