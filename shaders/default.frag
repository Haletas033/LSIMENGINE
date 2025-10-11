in vec3 Normal;
in vec3 crntPos;
in vec2 texCoord;
in mat3 TBN_out;

struct Light {
    vec4 lightColor;
    vec3 lightPos;
    float linear;
    float quadratic;
    float intensity;
};

#define MAX_LIGHTS 8
uniform Light lights[MAX_LIGHTS];
uniform vec4 meshColor;
uniform sampler2D tex0;
uniform sampler2D normal0;
uniform sampler2D tex1;
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
    return F0 + (1.0 - F0) * pow(1.0 - clamp(cosTheta,0.0,1.0), 5.0);
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

void main()
{
    //Create TBN
    vec3 T = normalize(TBN_out[0]);
    vec3 B = normalize(TBN_out[1]);
    vec3 N = normalize(TBN_out[2]);
    N = normalize(cross(T,B));
    B = cross(N,T);
    mat3 TBNn = mat3(T,B,N);

    //Get Normal information
    vec3 nMap = useNormalMap ? texture(normal0, texCoord).rgb * 2.0 - 1.0 : vec3(0.0,0.0,1.0);
    vec3 FragNormal = normalize(TBNn * nMap);

    vec3 albedo = useTexture ? texture(tex0, texCoord).rgb : meshColor.rgb;

    vec3 F0_local = F0;
    if(useTexture) {
        float specMap = pow(texture(tex1, texCoord).r, 2.2);
        F0_local = mix(F0, vec3(specMap), 1.0);
    }


    vec3 V = normalize(viewPos - crntPos);

    vec3 Lo = vec3(0.0);

    for(int i = 0; i < MAX_LIGHTS; ++i)
    {
        vec3 L = normalize(lights[i].lightPos - crntPos);
        vec3 H = normalize(V + L);

        float distance = length(lights[i].lightPos - crntPos);
        float attenuation = 1.0 / max(1.0 + lights[i].linear * distance + lights[i].quadratic * (distance*distance), 1e-6);

        float NdotL = max(dot(FragNormal,L),0.0);

        //Cook-Torrance
        vec3 F = fresnelSchlick(max(dot(H,V),0.0), F0_local);
        float D = DistributionGGX(FragNormal,H,roughness);
        float k = (roughness+1.0)*(roughness+1.0)/8.0;
        float G = GeometrySmith(FragNormal,V,L,k);

        vec3 numerator = D * G * F;
        float denominator = max(4.0 * max(dot(FragNormal,V),0.0) * NdotL, 1e-6);
        vec3 specular = numerator / denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        vec3 diffuse = kD * albedo / PI;

        vec3 radiance = (lights[i].lightColor.rgb * attenuation) * lights[i].intensity;
        Lo += (diffuse + specular) * radiance * NdotL;
    }

    FragColor = vec4(Lo,1.0);
}