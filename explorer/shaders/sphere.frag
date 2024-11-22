#version 330 core

uniform vec3 CameraPosition; // Camera position in world space
uniform vec3 LightPosition; // Light position in world space

uniform mat4 Projection;

in vec3 fColor; // Interpolated color from the geometry shader
in vec3 fPos; // World position of the sphere
in vec2 mapping; // Mapping coordinates for normal calculation

out vec4 FragColor;

void main()
{
    float lensqr = dot(mapping, mapping);
    if (lensqr > 1.0)
        discard;

    FragColor = vec4(fColor, 1.0);
}
