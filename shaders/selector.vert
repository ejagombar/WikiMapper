#version 330 core
layout(location = 0) in vec4 aRGBRadius;
layout(location = 1) in vec3 aPos;

out float vSize;
flat out uint vNodeID;

uniform uint nodeOffset;

// Make sure this matches EXACTLY the declaration in other shaders
layout(std140) uniform GlobalUniforms {
    mat4 projection;
    mat4 view;
    vec4 cameraPosition;
};

void main() {
    vSize = aRGBRadius.a * 10.0;
    // Fix the implicit cast by explicitly casting gl_VertexID to uint
    vNodeID = uint(gl_VertexID) + nodeOffset; // Use vertex ID as the node ID
    gl_Position = vec4(aPos, 1.0f);
}
