layout (triangles) in;
layout (triangle_strip, max_vertices  = 3) out;

out vec3 Normal;
out vec2 texCoord;
out mat3 TBN_out;
out vec3 crntPos;

in DATA{
    vec3 Normal;
    vec3 crntPos;
    vec2 texCoord;
    mat4 projection;
    mat4 model;
} data_in[];

void main() {
    vec3 edge0 = data_in[1].crntPos - data_in[0].crntPos;
    vec3 edge1 = data_in[2].crntPos - data_in[0].crntPos;
    vec2 deltaUV0 = data_in[1].texCoord - data_in[0].texCoord;
    vec2 deltaUV1 = data_in[2].texCoord - data_in[0].texCoord;

    float invDet = 1.0f / (deltaUV0.x * deltaUV1.y - deltaUV0.y * deltaUV1.x);

    vec3 tangent = vec3(invDet * (deltaUV1.y * edge0 - deltaUV0.y * edge1));
    vec3 bitangent = vec3(invDet * (-deltaUV1.x * edge0 - deltaUV0.x * edge1));

    vec3 T = normalize(vec3(vec4(tangent, 0.0f)));
    vec3 B = normalize(vec3(vec4(bitangent, 0.0f)));
    vec3 N = normalize(vec3(vec4(cross(edge0, edge1), 0.0f)));

    mat3 TBN = mat3(T, B, N);
    TBN_out = TBN;

    for (int i = 0; i < 3; ++i){
        gl_Position = data_in[i].projection * vec4(data_in[i].crntPos, 1.0f);
        Normal = data_in[i].Normal;
        texCoord = data_in[i].texCoord;
        crntPos = data_in[i].crntPos;
        EmitVertex();
    }

    EndPrimitive();
}
