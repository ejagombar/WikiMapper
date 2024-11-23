#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 PV; // Projection-View matrix,

void main() {
    gl_Position = PV * vec4(position, 1.0);
}
