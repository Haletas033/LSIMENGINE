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

uniform Light lights[MAX_LIGHTS];
uniform vec4 meshColor;
uniform sampler2D tex0; // diffuse map
uniform sampler2D tex1; // specular map
uniform bool useTexture;
uniform vec3 viewPos; // camera position

void main() {
    vec3 normal = normalize(Normal);
    vec4 result = vec4(0.0);

    for (int i = 0; i < MAX_LIGHTS; ++i) {
        vec3 lightDir = normalize(lights[i].lightPos - crntPos);
        float diffuse = max(dot(normal, lightDir), 0.0);
        float ambient = 0.2;

        float distance = length(lights[i].lightPos - crntPos);
        float attenuation = 1.0 / (1.0 + lights[i].linear * distance + lights[i].quadratic * distance * distance);

        vec4 baseColor = useTexture ? texture(tex0, texCoord) : meshColor;

        // Specular
        vec3 viewDir = normalize(viewPos - crntPos);
        vec3 reflectDir = reflect(-lightDir, normal);
        float shininess = 16.0;
        float specAmount = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        specAmount *= max(dot(normal, lightDir), 0.0);
        float specMap = texture(tex1, texCoord).r;
        vec3 specularColor = lights[i].lightColor.rgb * specAmount * specMap;

        vec3 lightContribution = (baseColor.rgb * lights[i].lightColor.rgb * (ambient + diffuse) + specularColor) * attenuation;
        result.rgb += lightContribution;
    }

    FragColor = vec4(result.rgb, 1.0);


    FragColor = result;
}