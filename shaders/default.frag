#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 crntPos;

in vec2 texCoord;

struct Light{
    vec4 lightColor;
    vec3 lightPos;
    float linear;
    float quadratic;
};

const int NUM_LIGHTS = 8;

uniform Light lights[NUM_LIGHTS];

uniform vec4 meshColor;

uniform sampler2D tex0;
uniform bool useTexture;

void main(){
    vec3 normal = normalize(Normal);

    vec4 result = vec4(0.0f);

    for (int i = 0; i < NUM_LIGHTS; ++i){
        vec3 lightDir = normalize(lights[i].lightPos - crntPos);

        float diffuse = max(dot(normal, lightDir), 0.0f);
        float ambient = 0.2f;

        float distance = length(lights[i].lightPos - crntPos);
        float attenuation = 1.0 / (1.0 + lights[i].linear * distance + lights[i].quadratic * distance * distance);

        vec4 baseColor = useTexture ? texture(tex0, texCoord) : meshColor;

        result += baseColor * lights[i].lightColor * (ambient + diffuse) * attenuation;
    }
    FragColor = result;
}