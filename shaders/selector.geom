#version 330 core
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

layout(std140) uniform GlobalUniforms {
    mat4 projection;
    mat4 view;
    vec4 cameraPosition;
};

in float vSize[];
flat in uint vNodeID[];

flat out uint fNodeID;
out vec2 mapping;

const float g_boxCorrection = 1.5;

void main() {
    fNodeID = vNodeID[0];

    vec3 cameraRight = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 cameraUp = vec3(view[0][1], view[1][1], view[2][1]);

    vec3 position = gl_in[0].gl_Position.xyz;
    vec3 center = position;
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
