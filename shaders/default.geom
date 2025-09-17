layout (triangles) in;
layout (triangle_strip, max_vertices  = 3) out;

out vec3 Normal;
out vec2 texCoord;
out mat3 TBN_out;

in DATA{
    vec3 Normal;
    vec3 crntPos;
    vec2 texCoord;
    mat4 projection;
    mat4 model;
} data_in[];

void main() {
    vec3 edge0 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 edge1 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec2 deltaUV0 = data_in[1].texCoord - data_in[0].texCoord;
    vec2 deltaUV1 = data_in[2].texCoord - data_in[0].texCoord;

    float invDet = 1.0f / (deltaUV0.x * deltaUV1.y - deltaUV0.y * deltaUV1.x);

    vec3 tangent = vec3(invDet * (deltaUV1.y * edge0 - deltaUV0.y * edge1));
    vec3 bitangent = vec3(invDet * (-deltaUV1.x * edge0 - deltaUV0.x * edge1));

    vec3 T = normalize(vec3(data_in[0].model * vec4(tangent, 0.0f)));
    vec3 B = normalize(vec3(data_in[0].model * vec4(bitangent, 0.0f)));
    vec3 N = normalize(vec3(data_in[0].model * vec4(cross(edge1, edge0), 0.0f)));

    mat3 TBN = mat3(T, B, N);
    TBN_out = TBN;


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
