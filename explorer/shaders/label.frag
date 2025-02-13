#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D tex;
uniform vec3 textColor;

void main()
{
    // float val = texture(tex, TexCoords).r;
    // if (val < 0.5) {
    //     discard;
    // }
    // vec4 sampled = vec4(1.0, 1.0, 1.0, val);
    // FragColor = vec4(1.0,1.0,1.0, 1.0) ;
    FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
