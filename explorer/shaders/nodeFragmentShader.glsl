#version 330 core

#extension GL_ARB_explicit_uniform_location : require
// Interpolated values from the vertex shaders
in vec2 UV;
in vec4 nodecolor;

// Output data
out vec4 color;

uniform sampler2D myTextureSampler;

void main() {
    // Output color = color of the texture at the specified UV
    vec4 texColor = texture(myTextureSampler, UV) * nodecolor;
    if (texColor.a < 0.2)
        discard;
    color = texColor;
}
