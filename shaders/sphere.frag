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

layout(std140) uniform MaterialProperties {
    float specularStrength;
    float shininess;
    float ambient;
};

in vec3 fColor; // Interpolated color from the geometry shader
in vec3 fPos; // World position of the sphere
in float fSize;
in vec2 mapping; // Mapping coordinates for normal calculation
flat in uint fNodeID;

out vec4 FragColor;

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
    for (int i = 0; i < pointLightCount; i++) {
        PointLight light = pointLight[i];

        if (!(light.constant > 0 && light.linear > 0 && light.quadratic > 0)) {
            continue;
        }

        vec3 lightDir = normalize(light.position - fPos);
        float distance = length(light.position - fPos);

        // Attenuation
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

        // Diffuse component
        float diffuse = max(dot(normal, lightDir), 0.0);

        // Specular component
        vec3 viewDir = normalize(cameraPosition.xyz - fPos);
        vec3 reflectDir = reflect(-lightDir, normal);
        float specular = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

        // Combine point light components
        pointLighting += attenuation * (diffuse + specularStrength * specular) * light.color;
    }

    // Combine all contributions
    vec3 lighting = ambient * fColor + globalLighting + pointLighting;

    FragColor = vec4(lighting * fColor, 1.0);
}
