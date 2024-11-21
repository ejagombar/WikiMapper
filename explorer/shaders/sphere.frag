#version 330 core

uniform vec3 CameraPosition; // Camera position in world space
uniform vec3 LightPosition; // Light position in world space

uniform mat4 Projection;

in vec3 fColor; // Interpolated color from the geometry shader
in vec3 sphereWorldPos; // World position of the sphere
in vec2 mapping; // Mapping coordinates for normal calculation
in mat3 billboardToWorld; // Matrix to transform normals to world space

out vec4 FragColor;

void main()
{
    float lensqr = dot(mapping, mapping);
    // if (lensqr > 1.0)
    //     discard;

    // Derive normal in local (circle) space
    vec3 normalLocal = normalize(vec3(mapping, sqrt(1.0 - lensqr)));

    // Transform normal to world space
    vec3 normal = normalize(billboardToWorld * normalLocal);

    // Compute light direction
    vec3 lightDir = normalize(LightPosition - sphereWorldPos);

    // Compute view direction
    vec3 viewDir = normalize(CameraPosition - sphereWorldPos);

    // Lambertian diffuse reflection
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular reflection (Phong model)
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

    // Combine components
    vec3 ambient = 0.1 * fColor; // Ambient term
    vec3 diffuse = diff * fColor; // Diffuse term
    vec3 specular = spec * vec3(1.0); // Specular term (white highlight)

    vec3 color = ambient + diffuse + specular;

    FragColor = vec4(color, 1.0);
}
