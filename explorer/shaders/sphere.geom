#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 Projection;
uniform mat4 View;

in gl_PerVertex
{
    vec4 gl_Position;
    float gl_PointSize;
    float gl_ClipDistance[];
} gl_in[];

in vec3 vColor[];
in float size[];

out vec3 fColor;
out vec2 mapping;

const float g_boxCorrection = 1.5;

void main()
{
    fColor = vColor[0];

    vec3 cameraRight = vec3(View[0][0], View[1][0], View[2][0]); // First column of View matrix
    vec3 cameraUp = vec3(View[0][1], View[1][1], View[2][1]); // Second column of View matrix

    vec3 center = vec3(gl_in[0].gl_Position);
    vec3 offsetX = size[0] * cameraRight; // Offset along the right vector
    vec3 offsetY = size[0] * cameraUp; // Offset along the up vector

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

// void main()
// {

// vec4 cameraCornerPos;
// //Bottom-left
// mapping = vec2(-1.0, -1.0) * g_boxCorrection;
// cameraSpherePos = vec3(vert[0].cameraSpherePos);
// sphereRadius = vert[0].sphereRadius;
// cameraCornerPos = vec4(vert[0].cameraSpherePos, 1.0);
// cameraCornerPos.xy += vec2(-vert[0].sphereRadius, -vert[0].sphereRadius) * g_boxCorrection;
// gl_Position = cameraToClipMatrix * cameraCornerPos;
// // gl_PrimitiveID = gl_PrimitiveIDIn;
// EmitVertex();
//
//     //Top-left
//     mapping = vec2(-1.0, 1.0) * g_boxCorrection;
//     cameraSpherePos = vec3(vert[0].cameraSpherePos);
//     sphereRadius = vert[0].sphereRadius;
//     cameraCornerPos = vec4(vert[0].cameraSpherePos, 1.0);
//     cameraCornerPos.xy += vec2(-vert[0].sphereRadius, vert[0].sphereRadius) * g_boxCorrection;
//     gl_Position = cameraToClipMatrix * cameraCornerPos;
//     gl_PrimitiveID = gl_PrimitiveIDIn;
//     EmitVertex();
//
//     //Bottom-right
//     mapping = vec2(1.0, -1.0) * g_boxCorrection;
//     cameraSpherePos = vec3(vert[0].cameraSpherePos);
//     sphereRadius = vert[0].sphereRadius;
//     cameraCornerPos = vec4(vert[0].cameraSpherePos, 1.0);
//     cameraCornerPos.xy += vec2(vert[0].sphereRadius, -vert[0].sphereRadius) * g_boxCorrection;
//     gl_Position = cameraToClipMatrix * cameraCornerPos;
//     gl_PrimitiveID = gl_PrimitiveIDIn;
//     EmitVertex();
//
//     //Top-right
//     mapping = vec2(1.0, 1.0) * g_boxCorrection;
//     cameraSpherePos = vec3(vert[0].cameraSpherePos);
//     sphereRadius = vert[0].sphereRadius;
//     cameraCornerPos = vec4(vert[0].cameraSpherePos, 1.0);
//     cameraCornerPos.xy += vec2(vert[0].sphereRadius, vert[0].sphereRadius) * g_boxCorrection;
//     gl_Position = cameraToClipMatrix * cameraCornerPos;
//     gl_PrimitiveID = gl_PrimitiveIDIn;
//     EmitVertex();
// }

// #extension GL_EXT_gpu_shader4 : enable
//
// layout(std140) uniform;
// layout(points) in;
// layout(triangle_strip, max_vertices = 4) out;
//
// uniform Projection
// {
//     mat4 cameraToClipMatrix;
// };
//
// in VertexData
// {
//     vec3 cameraSpherePos;
//     float sphereRadius;
// } vert[];
//
// out FragData
// {
//     flat vec3 cameraSpherePos;
//     flat float sphereRadius;
//     smooth vec2 mapping;
// };
//
// const float g_boxCorrection = 1.5;
