

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;
layout (location = 3) in mat4 instanceMatrix;

out DATA{
    vec3 Normal;
    vec3 crntPos;
    vec2 texCoord;
    mat4 projection;
    mat4 model;
} data_out;

uniform mat4 camMatrix;
uniform mat3 normalMatrix;

void main()
{
    data_out.crntPos = vec3(instanceMatrix * vec4(aPos, 1.0f));
    gl_Position = camMatrix * vec4(data_out.crntPos, 1.0);

    mat3 normalMat = mat3(transpose(inverse(instanceMatrix)));
    data_out.Normal = normalize(normalMat * aNormal);
    data_out.texCoord = aTex;
    data_out.projection = camMatrix;
    data_out.model = instanceMatrix;
}
