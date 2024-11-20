#version 330 core

// uniform vec3 cameraSpherePos;

in vec3 fColor;
in vec2 mapping;

out vec4 FragColor;

// void Impostor(out vec3 cameraPos, out vec3 cameraNormal)
// {
//     float lensqr = dot(mapping, mapping);
//     if (lensqr > 1.0)
//         discard;
//
//     cameraNormal = vec3(mapping, sqrt(1.0 - lensqr));
//     cameraPos = (cameraNormal * sphereRadius) + cameraSpherePos;
// }

void main()
{
    // vec3 cameraNormal;
    // vec3 cameraPos;
    //
    // Impostor(cameraPos, cameraNormal)

    float lensqr = dot(mapping, mapping);
    if (lensqr > 1.0)
        discard;

    FragColor = vec4(fColor, 1.0);
}
