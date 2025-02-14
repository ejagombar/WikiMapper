#version 330 core
in vec2 mapping;
flat in float fTexIndex;
out vec4 FragColor;

uniform sampler2DArray textAtlas;
uniform vec3 textColor;

void main()
{
    float alpha = texture(textAtlas, vec3(mapping, fTexIndex)).r;
    if (alpha < 0.5)
        discard;
    FragColor = vec4(textColor, alpha);
}
