layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;
layout (location = 3) in vec3 aTangent;

out vec3 crntPos;
out vec3 viewDir;
out vec3 Normal;
out vec2 texCoord;
out mat3 TBN;

uniform mat4 camMatrix;
uniform mat4 model;
uniform mat3 normalMatrix;
uniform vec3 viewPos;

void main()
{
    crntPos = vec3(model * vec4(aPos, 1.0f));


    viewDir = normalize(viewPos - crntPos);

    Normal = normalize(normalMatrix * aNormal);

    texCoord = aTex;

    vec3 T = normalize(mat3(model) * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    vec3 B = cross(N, T);

    TBN = mat3(T, B, N);

    gl_Position = camMatrix * vec4(crntPos, 1.0);
}
