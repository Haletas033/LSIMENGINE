in vec3 crntPos;
in vec3 viewDir;
in vec3 Normal;
in vec2 texCoord;
in mat3 TBN;

struct Light {
    int lightType;
    vec4 lightColor;
    vec3 lightPos;
    vec3 lightDir;
    float linear;
    float quadratic;
    float intensity;
    float spotAngle;
};

#define POINT_LIGHT 0
#define DIRECTIONAL_LIGHT 1
#define SPOT_LIGHT 2

uniform Light lights[MAX_LIGHTS];
uniform vec4 meshColor;

uniform vec4 ambientLightColour;
uniform float ambientLightIntensity;

//Maps
uniform sampler2D albedo;
uniform sampler2D normal;
uniform sampler2D specular;
uniform sampler2D emissive;

uniform float emissiveIntensity;

uniform bool useTexture;
uniform bool useNormalMap;
uniform vec3 viewPos;

//PBR
uniform vec3 F0;
uniform float roughness;

out vec4 FragColor;

#define PI 3.14159265359

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N,H),0.0);
    float NdotH2 = NdotH*NdotH;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return a2 / max(denom, 1e-6);
}

float GeometrySchlickGGX(float NdotV, float k)
{
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float k)
{
    return GeometrySchlickGGX(max(dot(N,V),0.0), k) * GeometrySchlickGGX(max(dot(N,L),0.0), k);
}

void main(){
    vec3 albedo = useTexture ? texture(albedo, texCoord).rgb : meshColor.rgb;

    vec3 V = normalize(viewPos - crntPos);

    vec3 Lo = vec3(0.0);

    vec3 F0_local = F0;
    vec3 emissiveMap = vec3(0.0);
    if (useTexture){
        float specMap = pow(texture(specular, texCoord).r, 2.2);
        F0_local = mix(F0, vec3(specMap), 1.0);

        emissiveMap = pow(texture(emissive, texCoord).rgb, vec3(2.2)) * emissiveIntensity;
    }

    for (int i = 0; i < MAX_LIGHTS; ++i){
        Light light = lights[i];

        if (light.intensity <= 0.0) continue;

        vec3 L;
        float attenuation = 1.0;

        if (light.lightType == DIRECTIONAL_LIGHT){
            L = normalize(-light.lightDir);
        }
        else {
            vec3 lightDir = light.lightPos - crntPos;
            float distance = length(lightDir);
            L = normalize(lightDir);

            attenuation = 1.0 / (1.0 + light.linear * distance + light.quadratic * (distance * distance));

            if (light.lightType == SPOT_LIGHT){
                float theta = dot(L, normalize(-light.lightDir));
                float outerCutoff = cos(light.spotAngle);
                float innerCutoff = cos(light.spotAngle * 0.9);
                float intensity = clamp((theta - outerCutoff) / (innerCutoff - outerCutoff), 0.0, 1.0);
                attenuation *= intensity;
            }
        }

        vec3 H = normalize(V + L);

        vec3 nMap = useNormalMap ? texture(normal, texCoord).rgb * 2.0 - 1.0 : vec3(0.0,0.0,1.0);
        vec3 N = normalize(TBN * nMap);

        float NdotL = max(dot(N, L), 0.0);

        //Cook torrance
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0_local);
        float D = DistributionGGX(N, H, roughness);
        float k = (roughness + 1.0) * (roughness + 1.0) / 8.0;
        float G = GeometrySmith(N, V, L, k);

        vec3 numerator = D * G * F;
        float denominator = max(4.0 * max(dot(N, V), 0.0) * NdotL, 1e-6);
        vec3 specular = numerator / denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        vec3 diffuse = kD * albedo / PI;

        vec3 radiance = light.lightColor.rgb * light.intensity * attenuation;


        Lo += (diffuse + specular) * radiance * NdotL;
    }

    vec3 ambient = albedo * ambientLightColour.rgb * ambientLightIntensity;
    //Tone-mapping
    vec3 hdrColor = Lo + emissiveMap + ambient;
    vec3 mapped = hdrColor / (hdrColor + vec3(1.0));
    FragColor = vec4(mapped, 1.0);
}