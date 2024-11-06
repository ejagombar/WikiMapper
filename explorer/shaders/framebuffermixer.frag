#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D originalTexture;
uniform sampler2D blurredTexture;
uniform float blurAmount; // 0 = no blur, 1 = full blur

void main()
{
    vec4 original = texture(originalTexture, TexCoords);
    vec4 blurred = texture(blurredTexture, TexCoords);
    FragColor = mix(original, blurred, blurAmount);
}
