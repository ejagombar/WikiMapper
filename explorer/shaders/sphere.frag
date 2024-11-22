#version 330 core

uniform vec3 CameraPosition; // Camera position in world space
uniform vec3 LightPosition; // Light position in world space

uniform mat4 Projection;
uniform mat4 View;

in vec3 fColor; // Interpolated color from the geometry shader
in vec3 fPos; // World position of the sphere
in float fSize;

in vec2 mapping; // Mapping coordinates for normal calculation

out vec4 FragColor;

void Impostor(out vec3 cameraPos, out vec3 cameraNormal)
{
    vec3 cameraSpherePos = vec3(View * vec4(fPos, 1.0));

    vec3 cameraPlanePos = vec3(mapping * fSize, 0.0) + cameraSpherePos;
    vec3 rayDirection = normalize(cameraPlanePos);

    float B = 2.0 * dot(rayDirection, -cameraSpherePos);
    float C = dot(cameraSpherePos, cameraSpherePos) - (fSize * fSize);

    float det = (B * B) - (4 * C);
    if (det < 0.0)
        discard;

    float sqrtDet = sqrt(det);
    float posT = (-B + sqrtDet) / 2;
    float negT = (-B - sqrtDet) / 2;

    float intersectT = min(posT, negT);
    cameraPos = rayDirection * intersectT;
    cameraNormal = normalize(cameraPos - cameraSpherePos);
}

void main()
{
    vec3 cameraPos;
    vec3 cameraNormal;

    Impostor(cameraPos, cameraNormal);

    vec4 clipPos = Projection * vec4(cameraPos, 1.0);
    float ndcDepth = clipPos.z / clipPos.w;
    gl_FragDepth = ((gl_DepthRange.diff * ndcDepth) + gl_DepthRange.near + gl_DepthRange.far) / 2.0;

    cameraNormal = mat3(transpose(View)) * cameraNormal;

    FragColor = vec4((cameraNormal + 1.0f) * 0.5f, 1.0f);
}
