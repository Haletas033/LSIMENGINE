#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 crntPos;

in vec2 texCoord;

uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec4 meshColor;

uniform float linear;
uniform float quadratic;

uniform sampler2D tex0;
uniform bool useTexture;

void main(){
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - crntPos);

    float diffuse = max(dot(normal, lightDir), 0.0f);
    float ambient = 0.2f;

    float distance = length(lightPos - crntPos);
    float attenuation = 1.0 / (1.0 + linear * distance + quadratic * distance * distance);

    vec4 baseColor = useTexture ? texture(tex0, texCoord) : meshColor;

    vec4 result = baseColor * lightColor * (ambient + diffuse) * attenuation;
    FragColor = result;
}