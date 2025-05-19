#version 330 core

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

layout(std140) uniform GlobalUniforms {
    mat4 projection;
    mat4 view;
    vec4 cameraPosition;
};

uniform mat3 normalMat;
uniform float time;
const float offsetSpeed = 0.4;

in vec3 vColor[];
in float vRadius[];

out vec3 gColor;

out vec3 point;
out vec3 axis;
out vec3 base;
out vec4 packedData;
out vec3 U;
out vec3 V;

#define end  packedData.xyz
#define gRadius packedData.w

void main()
{
    vec3 startPos = gl_in[0].gl_Position.xyz + vec3(sin(time + gl_in[0].gl_Position.z * offsetSpeed) * 0.04, sin(time + gl_in[0].gl_Position.y * offsetSpeed) * 0.055, sin(time + gl_in[0].gl_Position.x * offsetSpeed) * 0.07);
    vec3 endPos = gl_in[1].gl_Position.xyz + vec3(sin(time + gl_in[1].gl_Position.z * offsetSpeed) * 0.04, sin(time + gl_in[1].gl_Position.y * offsetSpeed) * 0.055, sin(time + gl_in[1].gl_Position.x * offsetSpeed) * 0.07);

    gRadius = min(vRadius[0], vRadius[1]);

    vec3 dir = normalize(startPos - endPos);

    vec3 camDir = normalize(cameraPosition.xyz - startPos);
    float b = dot(camDir, dir);

    vec3 ldir = b < 0.0 ? -dir : dir; // Ensure direction vector points correctly

    vec3 left = cross(camDir, ldir);
    vec3 up = cross(left, ldir);
    left = gRadius * normalize(left);
    up = gRadius * normalize(up);

    // Transform to model-view coordinates
    axis = normalize(normalMat * ldir);
    U = normalize(normalMat * up);
    V = normalize(normalMat * left);

    vec4 base4 = view * vec4(startPos - ldir, 1.0);
    base = base4.xyz / base4.w;

    vec4 end4 = view * vec4(endPos, 1.0);
    end = end4.xyz / end4.w;

    vec4 w0 = view * vec4(startPos + left - up, 1.0);
    vec4 w1 = view * vec4(startPos - left - up, 1.0);
    vec4 w2 = view * vec4(endPos + left - up, 1.0);
    vec4 w3 = view * vec4(endPos - left - up, 1.0);

    gColor = vColor[0];

    point = w0.xyz / w0.w;
    gl_Position = projection * w0;
    EmitVertex();

    point = w1.xyz / w1.w;
    gl_Position = projection * w1;
    EmitVertex();

    gColor = vColor[1];

    point = w2.xyz / w2.w;
    gl_Position = projection * w2;
    EmitVertex();

    point = w3.xyz / w3.w;
    gl_Position = projection * w3;
    EmitVertex();

    EndPrimitive();
}
