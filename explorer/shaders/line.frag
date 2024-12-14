#version 330 core

uniform vec3 CameraPosition; // Camera position in world space
uniform vec3 LightPosition; // Light position in world space
uniform vec3 LightColor; // Color of the point light
uniform vec3 GlobalLightColor; // Global light color

uniform mat4 Projection;
uniform mat4 View;

in vec3 fPos;
in vec2 mapping;
in vec3 cylinderAxis;
in float cylinderRadius;

out vec4 FragColor;

// void ImpostorCylinder(out vec3 worldPos, out vec3 normal)
// {
//     vec3 rayOrigin = vec3(View * vec4(fPos, 1.0)); // Ray origin in view space
//     vec3 rayDirection = normalize(vec3(mapping, 0.0)); // Ray direction in view space
//
//     vec3 cb = vec3(View * vec4(fPos, 1.0)); // Cylinder base in view space
//
//     vec3 oc = rayOrigin;
//     float card = dot(cylinderAxis, rayDirection);
//     float caoc = dot(cylinderAxis, oc);
//
//     float a = 1.0 - card * card;
//     float b = dot(oc, rayDirection) - caoc * card;
//     float c = dot(oc, oc) - caoc * caoc - cylinderRadius * cylinderRadius;
//
//     float h = b * b - a * c;
//     if (h < 0.0) discard;
//
//     float t = (-b - sqrt(h)) / a;
//     worldPos = rayOrigin + t * rayDirection;
//     normal = normalize(worldPos - cb - dot(worldPos - cb, cylinderAxis) * cylinderAxis);
// }

void ImpostorCylinder(out vec3 cameraPos, out vec3 cameraNormal) {
    vec3 cameraSpherePos = vec3(View * vec4(fPos, 1.0));

    vec3 cameraPlanePos = vec3(mapping * cylinderRadius, 0.0) + cameraSpherePos;
    vec3 rayDirection = normalize(cameraPlanePos);

    float card = dot(cylinderAxis, rayDirection);
    float caoc = dot(cylinderAxis, cameraSpherePos);

    float A = 1.0 - card * card;
    float B = dot(rayDirection, -cameraSpherePos) - caoc * card;
    float C = dot(cameraSpherePos, cameraSpherePos) - caoc * caoc - cylinderRadius * cylinderRadius;

    float det = (B * B) - (A * C);
    if (det < 0.0)
        discard;

    float sqrtDet = sqrt(det);
    float posT = (-B + sqrtDet);
    float negT = (-B - sqrtDet);

    float intersectT = min(posT, negT) / A;
    cameraPos = rayDirection * intersectT;
    cameraNormal = normalize(cameraPos - cameraSpherePos);
}

// void main()
// {
//     vec3 worldPos, cameraNormal;
//     ImpostorCylinder(worldPos, cameraNormal);
//
//     vec3 normal = normalize(mat3(transpose(View)) * cameraNormal);
//
//     vec3 lightDir = normalize(LightPosition - worldPos);
//     float diffuse = max(dot(normal, lightDir), 0.0);
//
//     vec3 reflectDir = reflect(-lightDir, normal);
//     vec3 viewDir = normalize(CameraPosition - fPos);
//     float specular = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
//
//     vec3 ambient = 0.2 * GlobalLightColor;
//     vec3 color = ambient + diffuse * LightColor + specular * LightColor;
//
//     FragColor = vec4(color, 1.0);
// }

void main()
{
    vec3 cameraPos;
    vec3 cameraNormal;

    ImpostorCylinder(cameraPos, cameraNormal);

    // Transform normals to world space
    // vec3 normal = normalize(mat3(transpose(View)) * cameraNormal);
    vec3 normal = cameraNormal;

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

    vec3 fColor = vec3(0.2, 0.8, 0.5);
    // Ambient component
    vec3 ambient = 0.2 * fColor;

    // Combine all contributions
    vec3 lighting = ambient + globalLighting + pointLighting;

    FragColor = vec4(lighting * fColor, 1.0);
}
