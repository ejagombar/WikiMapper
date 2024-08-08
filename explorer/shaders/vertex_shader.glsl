#version 330 core
layout(location = 4) in vec3 position;

uniform mat4 VP; // Model-View-Projection matrix, but without the Model (the position is in BillboardPos; the orientation depends on the camera)

void main() {
    gl_Position = VP * vec4(position, 1.0);
}
