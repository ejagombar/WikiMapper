#version 330 core
flat in uint fNodeID;
in vec2 mapping;
out vec4 FragColor;

void main() {
    float dist = length(mapping);
    if (dist > 1.0)
        discard;

    uint id = fNodeID + 1u;

    FragColor = vec4(
            float(id & 0xFFu) / 255.0,
            float((id >> 8u) & 0xFFu) / 255.0,
            float((id >> 16u) & 0xFFu) / 255.0,
            1.0
        );
}
