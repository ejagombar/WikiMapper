// #version 330 core
//
// #extension GL_ARB_explicit_uniform_location : require
//
// in vec2 UV;
// in vec4 nodecolor;
//
// out vec4 FragColor;
//
// uniform sampler2D myTextureSampler;
//
// void main() {
//     if (nodecolor.a < 0.2)
//         discard;
//
//     FragColor = texture(myTextureSampler, UV) * nodecolor;
// }
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{
    vec4 colour = texture(texture1, TexCoords);

    if (colour.a < 0.2)
        discard;

    FragColor = colour;
}
