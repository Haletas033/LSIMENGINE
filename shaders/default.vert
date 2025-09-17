

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;

out DATA{
    vec3 Normal;
    vec3 crntPos;
    vec2 texCoord;
    mat4 projection;
} data_out;

uniform mat4 camMatrix;
uniform mat4 model;
uniform mat3 normalMatrix;

void main()
{
    data_out.crntPos = vec3(model * vec4(aPos, 1.0f));
    gl_Position = camMatrix * vec4(data_out.crntPos, 1.0);

    data_out.Normal = normalMatrix * aNormal;
    data_out.texCoord = aTex;
    data_out.projection = camMatrix;
}
