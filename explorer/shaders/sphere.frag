#version 330 core

// #extension GL_ARB_explicit_uniform_location : require

in vec2 TexCoords;
in vec4 sphereColor;

out vec4 FragColor;

uniform sampler2D texture1;

void main()
{
    vec4 colour = texture(texture1, TexCoords);

    if (colour.a < 0.2)
        discard;

    FragColor = sphereColor;
}
