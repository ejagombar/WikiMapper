#version 330 core

struct PointLight {
    vec3 position;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
};

layout(std140) uniform EnvironmentUniforms {
    vec3 globalLightColor;
    vec3 globalLightDir;
    int pointLightCount;
    PointLight pointLight[4];
};

layout(std140) uniform GlobalUniforms {
    mat4 projection;
    mat4 view;
    vec4 cameraPosition;
};

in vec3 fColor; // Interpolated color from the geometry shader
in vec3 fPos; // World position of the sphere
in float fSize;
in vec2 mapping; // Mapping coordinates for normal calculation

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

void Impostor(out vec3 cameraPos, out vec3 cameraNormal)
{
    vec3 cameraSpherePos = vec3(view * vec4(fPos, 1.0));

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

    vec4 clipPos = projection * vec4(cameraPos, 1.0);
    float ndcDepth = clipPos.z / clipPos.w;
    gl_FragDepth = ((gl_DepthRange.diff * ndcDepth) + gl_DepthRange.near + gl_DepthRange.far) / 2.0;

    // Transform normals to world space
    vec3 normal = normalize(mat3(transpose(view)) * cameraNormal);

    float globalDiffuse = max(dot(normal, normalize(globalLightDir)), 0.0);
    vec3 globalLighting = globalDiffuse * globalLightColor;

    // Accumulate point light contributions
    vec3 pointLighting = vec3(0.0);
    for (int i = 0; i < pointLightCount; ++i) {
        PointLight light = pointLight[i];
        vec3 lightDir = normalize(light.position - fPos);
        float distance = length(light.position - fPos);

        // Attenuation
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

        // Diffuse component
        float diffuse = max(dot(normal, lightDir), 0.0);

        // Specular component
        vec3 viewDir = normalize(cameraPosition.xyz - fPos);
        vec3 reflectDir = reflect(-lightDir, normal);
        float specularStrength = 0.9;
        float shininess = 128.0;
        float specular = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

        // Combine point light components
        pointLighting += attenuation * (diffuse + specularStrength * specular) * light.color;
    }

    // Ambient component
    vec3 ambient = 0.2 * fColor;

    // Combine all contributions
    vec3 lighting = ambient + globalLighting + pointLighting;

    vec3 result = lighting * fColor;

    float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 0.3)
        BrightColor = vec4(result, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);

    FragColor = vec4(result, 1.0);
}
