#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D sceneTexture;
uniform sampler2D bloomTexture;
uniform float intensity;

void main() {
    vec3 scene = texture(sceneTexture, TexCoords).rgb;
    vec3 bloom = texture(bloomTexture, TexCoords).rgb;
    FragColor = vec4(scene + bloom * intensity, 1.0);
}
