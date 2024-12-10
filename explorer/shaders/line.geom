#version 330 core

// layout(line_strip, max_vertices = 2) in;
layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 Projection;
uniform mat4 View;
uniform vec3 CameraPosition;

out vec2 mapping;

void main()
{
    float LineWidth = 0.1; // Thickness of the line
    // Line segment start and end in world space
    vec3 start = gl_in[0].gl_Position.xyz;
    vec3 end = gl_in[1].gl_Position.xyz;

    // Direction of the line
    vec3 lineDirection = normalize(end - start);

    // Center of the line
    vec3 center = (start + end) * 0.5;

    // Vector from the camera to the center of the line
    vec3 cameraToLine = normalize(center - CameraPosition);

    // Compute a vector perpendicular to the line and the camera direction
    vec3 perpendicular = normalize(cross(lineDirection, cameraToLine));
    vec3 offset = (LineWidth * 0.5) * perpendicular;

    // Projection * View matrix
    mat4 PV = Projection * View;

    // Emit vertices for the rectangle
    mapping = vec2(0.0, 0.0);
    gl_Position = PV * vec4(start - offset, 1.0);
    EmitVertex();

    mapping = vec2(0.0, 1.0);
    gl_Position = PV * vec4(start + offset, 1.0);
    EmitVertex();

    mapping = vec2(1.0, 0.0);
    gl_Position = PV * vec4(end - offset, 1.0);
    EmitVertex();

    mapping = vec2(1.0, 1.0);
    gl_Position = PV * vec4(end + offset, 1.0);
    EmitVertex();

    EndPrimitive();
}
