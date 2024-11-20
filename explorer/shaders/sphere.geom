#version 330 core

layout(points) in;
layout(line_strip, max_vertices = 2) out;

in gl_PerVertex
{
    vec4 gl_Position;
    float gl_PointSize;
    float gl_ClipDistance[];
} gl_in[];

in vec3 vColor[];
in float size[];

out vec3 fColor;

void main()
{
    fColor = vColor[0];

    gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(0.1, 0.0, 0.0, 0.0);
    EmitVertex();

    EndPrimitive();
}

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
//
// void main()
// {
//     vec4 cameraCornerPos;
//     //Bottom-left
//     mapping = vec2(-1.0, -1.0) * g_boxCorrection;
//     cameraSpherePos = vec3(vert[0].cameraSpherePos);
//     sphereRadius = vert[0].sphereRadius;
//     cameraCornerPos = vec4(vert[0].cameraSpherePos, 1.0);
//     cameraCornerPos.xy += vec2(-vert[0].sphereRadius, -vert[0].sphereRadius) * g_boxCorrection;
//     gl_Position = cameraToClipMatrix * cameraCornerPos;
//     gl_PrimitiveID = gl_PrimitiveIDIn;
//     EmitVertex();
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
