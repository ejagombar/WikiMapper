#version 330 core

layout(location = 0) in vec3 Start;
layout(location = 1) in vec3 End;
layout(location = 2) in vec3 Color;
layout(location = 3) in float Radius;

out vec3 cylinder_color_in;
out vec3 cylinder_direction_in;
out float cylinder_radius_in;
out float cylinder_ext_in;

void main()
{
    cylinder_color_in = Color;
    cylinder_direction_in = normalize(Start - End);
    cylinder_radius_in = Radius;
    cylinder_ext_in = distance(End, Start) / 2;
    gl_Position = vec4((Start + End) / 2, 1.0);
}
