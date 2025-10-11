layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in DATA {
    vec3 Normal;
    vec3 crntPos;
    vec2 texCoord;
    mat4 projection;
    mat4 model;
} data_in[];

out vec3 Normal;
out vec3 crntPos;
out vec2 texCoord;
out mat3 TBN_out;

void main() {
    vec3 edge0 = data_in[1].crntPos - data_in[0].crntPos;
    vec3 edge1 = data_in[2].crntPos - data_in[0].crntPos;

    vec2 deltaUV0 = data_in[1].texCoord - data_in[0].texCoord;
    vec2 deltaUV1 = data_in[2].texCoord - data_in[0].texCoord;

    float det = deltaUV0.x * deltaUV1.y - deltaUV0.y * deltaUV1.x;
    if(abs(det) < 1e-6) det = 1e-6;
    float invDet = 1.0 / det;

    vec3 triTangent = normalize(invDet * (deltaUV1.y * edge0 - deltaUV0.y * edge1));
    vec3 triBitangent = normalize(invDet * (-deltaUV1.x * edge0 + deltaUV0.x * edge1));

    for(int i = 0; i < 3; ++i) {
        vec3 N = normalize(data_in[i].Normal);

        vec3 T = normalize(triTangent - N * dot(N, triTangent));
        vec3 B = cross(N, T);

        TBN_out = mat3(T, B, N);

        gl_Position = data_in[i].projection * vec4(data_in[i].crntPos, 1.0);
        Normal = N;
        crntPos = data_in[i].crntPos;
        texCoord = data_in[i].texCoord;
        EmitVertex();
    }

    EndPrimitive();
}
