layout (triangles) in;
layout (triangle_strip, max_vertices  = 3) out;

out vec3 Normal;
out vec2 texCoord;

in DATA{
    vec3 Normal;
    vec3 crntPos;
    vec2 texCoord;
    mat4 projection;
} data_in[];

void main() {
    gl_Position = data_in[0].projection * vec4(data_in[0].crntPos, 1.0f);
    Normal = data_in[0].Normal;
    texCoord = data_in[0].texCoord;
    EmitVertex();

    gl_Position = data_in[1].projection * vec4(data_in[1].crntPos, 1.0f);
    Normal = data_in[1].Normal;
    texCoord = data_in[1].texCoord;
    EmitVertex();

    gl_Position = data_in[2].projection * vec4(data_in[2].crntPos, 1.0f);
    Normal = data_in[2].Normal;
    texCoord = data_in[2].texCoord;
    EmitVertex();

    EndPrimitive();
}
