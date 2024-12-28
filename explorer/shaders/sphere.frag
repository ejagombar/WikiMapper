#version 330 core

// Light structure definition
struct PointLight {
    vec3 Position; // Position of the light in world space
    vec3 Color; // Color of the point light
    float Constant; // Constant attenuation factor
    float Linear; // Linear attenuation factor
    float Quadratic; // Quadratic attenuation factor
};

layout(std140) uniform GlobalUniforms {
    mat4 Projection;
    mat4 View;
    vec4 CameraPosition;
};

layout(std140) uniform EnvironmentUniforms {
    vec3 GlobalLightColor;
    vec3 GlobalLightDir;
    int NumPointLights;
    PointLight PointLights[16];
};

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

    // Global light contribution
    float globalDiffuse = max(dot(normal, normalize(GlobalLightDir)), 0.0);
    vec3 globalLighting = globalDiffuse * GlobalLightColor;

    // Accumulate point light contributions
    vec3 pointLighting = vec3(0.0);
    for (int i = 0; i < NumPointLights; ++i) {
        PointLight light = PointLights[i];
        vec3 lightDir = normalize(light.Position - fPos);
        float distance = length(light.Position - fPos);

        // Attenuation
        float attenuation = 1.0 / (light.Constant + light.Linear * distance + light.Quadratic * (distance * distance));

        // Diffuse component
        float diffuse = max(dot(normal, lightDir), 0.0);

        // Specular component
        vec3 viewDir = normalize(CameraPosition.xyz - fPos);
        vec3 reflectDir = reflect(-lightDir, normal);
        float specularStrength = 0.9;
        float shininess = 128.0;
        float specular = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

        // Combine point light components
        pointLighting += attenuation * (diffuse + specularStrength * specular) * light.Color;
    }

    // Ambient component
    vec3 ambient = 0.2 * fColor;

    // Combine all contributions
    vec3 lighting = ambient + globalLighting + pointLighting;

    FragColor = vec4(lighting * fColor, 1.0);
}

