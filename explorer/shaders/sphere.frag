#version 330 core

uniform vec3 CameraPosition; // Camera position in world space
uniform vec3 LightPosition; // Light position in world space
uniform vec3 LightColor; // Color of the point light
uniform vec3 GlobalLightColor; // Color of the global light (e.g., sunlight)

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

    float B = dot(rayDirection, -cameraSpherePos);
    float C = dot(cameraSpherePos, cameraSpherePos) - (fSize * fSize);

    float det = (B * B) - C;
    if (det < 0.0)
        discard;

    float sqrtDet = sqrt(det);
    float posT = -B + sqrtDet;
    float negT = -B - sqrtDet;

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

    // Transform normals to world space
    vec3 normal = normalize(mat3(transpose(View)) * cameraNormal);

    // Global light (sunlight) contribution
    vec3 globalLightDir = normalize(vec3(0.0, 1.0, 0.0)); // Global light direction
    float globalDiffuse = max(dot(normal, globalLightDir), 0.0);
    vec3 globalLighting = globalDiffuse * GlobalLightColor;

    // Point light contribution
    vec3 lightDir = normalize(LightPosition - fPos);
    float distance = length(LightPosition - fPos);

    // Attenuation
    float attenuation = 1.0 / (1 + 0.03 * distance + 0.012 * distance * distance);

    // Diffuse component
    float diffuse = max(dot(normal, lightDir), 0.0);

    // Specular component
    vec3 viewDir = normalize(CameraPosition - fPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float specularStrength = 0.9;
    float shininess = 128.0;
    float specular = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    // Combine point light components with attenuation
    vec3 pointLighting = attenuation * (diffuse + specularStrength * specular) * LightColor;

    // Ambient component
    vec3 ambient = 0.2 * fColor;

    // Combine all contributions
    vec3 lighting = ambient + globalLighting + pointLighting;

    FragColor = vec4(lighting * fColor, 1.0);
}
