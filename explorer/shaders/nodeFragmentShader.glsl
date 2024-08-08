#version 330 core

#extension GL_ARB_explicit_uniform_location : require

// Interpolated values from the vertex shader
in vec2 UV;
in vec4 nodecolor;
in vec3 Normal; // Interpolated normal
in vec3 WorldPos; // Interpolated world position

// Output data
out vec4 color;

// Uniforms
uniform sampler2D myTextureSampler;
uniform vec3 LightPosition_worldspace; // Position of the light
uniform vec3 LightColor; // Color of the light
uniform float LightPower; // Intensity of the light
uniform vec3 ViewPosition_worldspace; // Position of the camera/viewer

void main() {
    // Sample texture color
    vec4 texColor = texture(myTextureSampler, UV) * nodecolor;

    // Discard transparent fragments
    if (texColor.a < 0.2)
        discard;

    // Calculate light direction and distance
    vec3 lightDir = normalize(LightPosition_worldspace - WorldPos);
    float distance = length(LightPosition_worldspace - WorldPos);

    // Calculate normal (assuming it's normalized already)
    vec3 normal = normalize(Normal);

    // Lambertian reflectance (diffuse shading)
    float diffuse = max(dot(normal, lightDir), 0.0);

    // Phong reflection model
    vec3 viewDir = normalize(ViewPosition_worldspace - WorldPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32); // Specular exponent

    // Combine lighting components
    vec3 ambient = 0.1 * LightColor; // Ambient light
    vec3 diffuseColor = LightColor * diffuse;
    vec3 specular = LightColor * spec;

    // Calculate final color
    vec3 finalColor = (ambient + diffuseColor + specular) * vec3(texColor);

    // Apply light attenuation
    float attenuation = 1.0 / (1.0 + 0.1 * distance + 0.01 * distance * distance);
    finalColor *= LightPower;

    // Output final color
    color = vec4(finalColor, texColor.a);
}

