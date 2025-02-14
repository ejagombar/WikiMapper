#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform float time;
uniform float vHeight;

const float offsetSpeed = 0.4;

layout(std140) uniform GlobalUniforms {
    mat4 projection;
    mat4 view;
    vec4 cameraPosition;
};

in float vWidth[];
flat in float vTexIndex[];

out vec2 mapping;
flat out float fTexIndex;

const float g_boxCorrection = 1.5;

void main() {
    fTexIndex = vTexIndex[0];

    // Method 2
    vec3 center = gl_in[0].gl_Position.xyz +
            vec3(sin(time + gl_in[0].gl_Position.z * offsetSpeed) * 0.04,
                sin(time + gl_in[0].gl_Position.y * offsetSpeed) * 0.055,
                sin(time + gl_in[0].gl_Position.x * offsetSpeed) * 0.07);

    vec3 cameraDir = normalize(center - cameraPosition.xyz);
    vec3 cameraRight = normalize(cross(vec3(0.0, 1.0, 0.0), cameraDir));
    vec3 cameraUp = cross(cameraDir, cameraRight);

    center -= cameraDir * 1;

    float dis = distance(cameraPosition.xyz, center);
    float size = 1;
    if (dis < 20.0)
        size *= 0.05 * dis;
    if (dis < 1.5)
        size = 0;

    vec3 offsetX = vWidth[0] * cameraRight * size;
    vec3 offsetY = vHeight * cameraUp * size;

    // Method 1
    // vec3 cameraRight = vec3(view[0][0], view[1][0], view[2][0]);
    // vec3 cameraUp = vec3(view[0][1], view[1][1], view[2][1]);
    // vec3 cameraForward = normalize(cameraPosition.xyz - gl_in[0].gl_Position.xyz);
    //
    // vec3 center = gl_in[0].gl_Position.xyz + vec3(sin(time + gl_in[0].gl_Position.z * offsetSpeed) * 0.04, sin(time + gl_in[0].gl_Position.y * offsetSpeed) * 0.055, sin(time + gl_in[0].gl_Position.x * offsetSpeed) * 0.07);
    //
    // float dis = distance(cameraPosition.xyz, center);
    //
    // float size = 1;
    // if (dis < 20.0)
    //     size *= 0.05 * dis;
    // if (dis < 1.5)
    //     size = 0;
    //
    // vec3 offsetX = vWidth[0] * cameraRight * size;
    // vec3 offsetY = vHeight * cameraUp * size;
    //
    // center += cameraForward * 1;

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

// void main() {
//     // Add a slight time-based jitter to the center.
//     vec3 center = gl_in[0].gl_Position.xyz +
//             vec3(sin(time + gl_in[0].gl_Position.z * offsetSpeed) * 0.04,
//                 sin(time + gl_in[0].gl_Position.y * offsetSpeed) * 0.055,
//                 sin(time + gl_in[0].gl_Position.x * offsetSpeed) * 0.07);
//
//     // Billboard: compute camera-oriented axes.
//     vec3 cameraDir = normalize(center - cameraPosition.xyz);
//     vec3 cameraRight = normalize(cross(vec3(0.0, 1.0, 0.0), cameraDir));
//     vec3 cameraUp = cross(cameraDir, cameraRight);
//
//     // Offset the center slightly back.
//     center -= cameraDir * 1.0;
//
//     float dis = distance(cameraPosition.xyz, center);
//     float size = 1.0;
//     if (dis < 20.0)
//         size *= 0.05 * dis;
//     if (dis < 1.5)
//         size = 0.0;
//
//     vec3 offsetX = vWidth[0] * cameraRight * size;
//     vec3 offsetY = vHeight * cameraUp * size;
//
//     mat4 PV = projection * view;
//     fTexIndex = vTexIndex[0];
//
//     mapping = vec2(0.0, 0.0);
//     gl_Position = PV * vec4(center - offsetX - offsetY, 1.0);
//     EmitVertex();
//
//     mapping = vec2(0.0, 1.0);
//     gl_Position = PV * vec4(center - offsetX + offsetY, 1.0);
//     EmitVertex();
//
//     mapping = vec2(1.0, 0.0);
//     gl_Position = PV * vec4(center + offsetX - offsetY, 1.0);
//     EmitVertex();
//
//     mapping = vec2(1.0, 1.0);
//     gl_Position = PV * vec4(center + offsetX + offsetY, 1.0);
//     EmitVertex();
//
//     EndPrimitive();
// }
