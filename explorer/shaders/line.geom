#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

layout(std140) uniform GlobalUniforms {
    mat4 Projection;
    mat4 View;
    vec4 CameraPosition;
};

uniform vec4 lightPos;
uniform mat3 NormalMat;

in vec3 vEnd[];
in vec3 vColor[];
in float vRadius[];

flat out vec3 cylinder_color;
flat out vec3 lightDir;

out vec3 packed_data_0;
out vec3 packed_data_1;
out vec3 packed_data_2;
out vec4 packed_data_3;
out vec3 packed_data_4;
out vec3 packed_data_5;

#define point ( packed_data_0 )
#define axis ( packed_data_1 )
#define base ( packed_data_2 )
#define end ( packed_data_3.xyz )
#define U ( packed_data_4.xyz )
#define V ( packed_data_5.xyz )
#define cylinder_radius (packed_data_3.w)

void main()
{
    vec3 Start = gl_in[0].gl_Position.xyz;

    cylinder_color = vColor[0];
    cylinder_radius = vRadius[0];

    vec3 dir = normalize(Start - vEnd[0]);
    lightDir = normalize(lightPos.xyz);

    vec3 cam_dir = normalize(CameraPosition.xyz - Start);
    float b = dot(cam_dir, dir);

    vec3 ldir = b < 0.0 ? -dir : dir; // Ensure direction vector points correctly

    vec3 left = cross(cam_dir, ldir);
    vec3 up = cross(left, ldir);
    left = cylinder_radius * normalize(left);
    up = cylinder_radius * normalize(up);

    // Transform to model-view coordinates
    axis = normalize(NormalMat * ldir);
    U = normalize(NormalMat * up);
    V = normalize(NormalMat * left);

    vec4 base4 = View * vec4(Start - ldir, 1.0);
    base = base4.xyz / base4.w;

    vec4 end4 = View * vec4(vEnd[0], 1.0);
    end = end4.xyz / end4.w;

    vec4 w0 = View * vec4(Start + left - up, 1.0);
    vec4 w1 = View * vec4(Start - left - up, 1.0);
    vec4 w2 = View * vec4(vEnd[0] + left - up, 1.0);
    vec4 w3 = View * vec4(vEnd[0] - left - up, 1.0);

    // Emit vertices for the cylinder sides
    point = w0.xyz / w0.w;
    gl_Position = Projection * w0;
    EmitVertex();

    point = w1.xyz / w1.w;
    gl_Position = Projection * w1;
    EmitVertex();

    point = w2.xyz / w2.w;
    gl_Position = Projection * w2;
    EmitVertex();

    point = w3.xyz / w3.w;
    gl_Position = Projection * w3;
    EmitVertex();

    EndPrimitive();
}
