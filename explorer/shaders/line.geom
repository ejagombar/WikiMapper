#version 330 core

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 Projection;
uniform mat4 View;
uniform vec3 CameraPosition;

out vec3 fPos; // World position for the fragment shader
out vec2 mapping; // UV-like coordinates for ray intersection
out vec3 cylinderAxis; // Axis of the cylinder
out float cylinderRadius;

void main()
{
    float cylinderWidth = 0.1;

    vec3 start = gl_in[0].gl_Position.xyz;
    vec3 end = gl_in[1].gl_Position.xyz;

    vec3 center = (start + end) * 0.5;
    vec3 lineDirection = normalize(end - start);
    vec3 cameraToLine = normalize(center - CameraPosition);

    vec3 perpendicular = normalize(cross(lineDirection, cameraToLine));
    vec3 offset = (cylinderWidth * 0.5) * perpendicular;

    mat4 PV = Projection * View;

    fPos = center;
    cylinderAxis = lineDirection;
    cylinderRadius = cylinderWidth * 0.5f;

    gl_Position = PV * vec4(start - offset, 1.0);
    mapping = vec2(-1.0f, -1.0f);
    EmitVertex();

    gl_Position = PV * vec4(start + offset, 1.0);
    mapping = vec2(-1.0f, 1.0f);
    EmitVertex();

    gl_Position = PV * vec4(end - offset, 1.0);
    mapping = vec2(1.0f, -1.0f);
    EmitVertex();

    gl_Position = PV * vec4(end + offset, 1.0);
    mapping = vec2(1.0f, 1.0f);
    EmitVertex();

    EndPrimitive();
}
