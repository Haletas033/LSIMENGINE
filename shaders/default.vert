layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;
layout (location = 3) in vec3 aTangent;

// Instance data
layout (location = 4) in mat4 instanceModel;
layout (location = 8) in mat3 instanceNormalMatrix;

out vec3 crntPos;
out vec3 viewDir;
out vec3 Normal;
out vec2 texCoord;
out mat3 TBN;

uniform mat4 camMatrix;
uniform vec3 viewPos;

void main() {
    crntPos = vec3(instanceModel * vec4(aPos, 1.0));

    viewDir = normalize(viewPos - crntPos);

    Normal = normalize(instanceNormalMatrix * aNormal);

    texCoord = aTex;

    vec3 T = normalize(mat3(instanceModel) * aTangent);
    vec3 N = normalize(instanceNormalMatrix * aNormal);
    vec3 B = normalize(cross(N, T));

    TBN = mat3(T, B, N);

    gl_Position = camMatrix * vec4(crntPos, 1.0);
}