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

in vec3 gColor;

in vec3 point;
in vec3 axis;
in vec3 base;
in vec3 U;
in vec3 V;
in vec4 packedData;

#define end_cyl packedData.xyz
#define radius ( packedData.w )

out vec4 out_Color;

vec4 ComputeColorForLight(vec3 N, vec3 L, vec4 ambient, vec4 diffuse, vec4 specular, vec4 color, vec3 viewDir) {
    float NdotL = max(dot(N, L), 0.0);
    vec4 result = ambient * color;

    if (NdotL > 0.0) {
        result += diffuse * NdotL * color;

        // Specular lighting
        vec3 R = reflect(-L, N);
        float spec = pow(max(dot(viewDir, R), 0.0), 32.0); // Shininess factor
        result += specular * spec * color;
    }

    return result;
}

void main()
{
    vec4 color = vec4(gColor, 0.3);
    // vec4 color = vec4(1.0f, 1.0f, 1.0f, 0.3);
    vec3 ray_target = point;
    vec3 ray_origin = vec3(0.0);
    vec3 ray_direction = normalize(ray_origin - ray_target);
    mat3 basis = mat3(U, V, axis);

    vec3 diff = ray_target - 0.5 * (base + end_cyl);
    vec3 P = diff * basis;

    // angle (cos) between cylinder cylinder_axis and ray direction
    float dz = dot(axis, ray_direction);
    float radius2 = radius * radius;

    vec3 D = vec3(dot(U, ray_direction), dot(V, ray_direction), dz);

    float a0 = P.x * P.x + P.y * P.y - radius2;
    float a1 = P.x * D.x + P.y * D.y;
    float a2 = D.x * D.x + D.y * D.y;

    float d = a1 * a1 - a0 * a2;

    if (d < 0.0)
        discard;

    float dist = (-a1 + sqrt(d)) / a2;

    // point of intersection on cylinder surface
    vec3 new_point = ray_target + dist * ray_direction;
    vec3 tmp_point = new_point - base;
    vec3 normal = normalize(tmp_point - axis * dot(tmp_point, axis));

    vec2 clipZW = new_point.z * projection[2].zw + projection[3].zw;
    float depth = 0.5 + 0.5 * clipZW.x / clipZW.y;

    gl_FragDepth = depth;

    vec3 viewDir = normalize(vec3(cameraPosition) - new_point);

    vec4 final_color = vec4(0.0);

    // Global light contributions
    vec3 lightDir = normalize(-globalLightDir);
    final_color += ComputeColorForLight(
            normal, lightDir,
            vec4(0.12, 0.12, 0.12, 1.0) * vec4(globalLightColor, 1.0), // ambient
            vec4(1.0, 1.0, 1.0, 1.0) * vec4(globalLightColor, 1.0), // diffuse
            vec4(0.5, 0.5, 0.5, 1.0), // specular
            color, viewDir
        );

    // Point light contributions
    for (int i = 0; i < pointLightCount; i++) {
        PointLight light = pointLight[i];

        if (!(light.constant > 0 && light.linear > 0 && light.quadratic > 0)) {
            continue;
        }

        vec3 toLight = light.position - new_point;
        float distance = length(toLight);
        vec3 lightDir = normalize(toLight);

        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

        final_color += ComputeColorForLight(
                normal, lightDir,
                vec4(0.1, 0.1, 0.1, 1.0) * vec4(light.color, 1.0) * attenuation, // ambient
                vec4(1.0, 1.0, 1.0, 1.0) * vec4(light.color, 1.0) * attenuation, // diffuse
                vec4(0.3, 0.3, 0.3, 1.0) * attenuation, // specular
                color, viewDir
            );
    }

    out_Color = final_color;
}
