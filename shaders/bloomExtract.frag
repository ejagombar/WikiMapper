#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D sceneTexture;
uniform float threshold;
uniform float softKnee;

void main() {
    vec3 color = texture(sceneTexture, TexCoords).rgb;
    float luminance = dot(color, vec3(0.2126, 0.7152, 0.0722));

    // Soft knee for smooth transition around threshold
    float knee = threshold * softKnee;
    float soft = luminance - threshold + knee;
    soft = clamp(soft / (2.0 * knee + 0.0001), 0.0, 1.0);
    soft = soft * soft;

    float contribution = max(soft, step(threshold, luminance));
    FragColor = vec4(color * contribution, 1.0);
}
