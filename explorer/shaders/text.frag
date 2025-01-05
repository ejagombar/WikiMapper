#version 330 core
in vec2 TexCoords;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

uniform sampler2D tex;
uniform vec3 textColor;

void main()
{
    float val = texture(tex, TexCoords).r;
    if (val < 0.1) {
        discard;
    }
    vec4 sampled = vec4(1.0, 1.0, 1.0, val);
    FragColor = vec4(textColor, 1.0) * sampled;

    BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
