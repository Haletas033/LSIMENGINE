#version 450

const float PI = 3.141592653589793;

layout(set = 0, binding = 0) uniform UBO {
        mat4 model;
        mat4 view;
        mat4 proj;
        mat4 invView;
        mat4 invProj;
        vec3 cameraPos;
} ubo;

layout(set = 0, binding = 1) uniform sampler2D inAlbedo;
layout(set = 0, binding = 2) uniform sampler2D inNormal;
layout(set = 0, binding = 3) uniform sampler2D inEmissive;
layout(set = 0, binding = 4) uniform sampler2D inDepth;

layout(location = 0) in vec2 fragUV;
layout(location = 0) out vec4 fragColor;

const vec3 lightDir = vec3(0, 1, 0);
const vec3 lightColor = vec3(1, 1, 1);

float D_GGX(float NdotH, float roughness) {
        float roughness2 = roughness * roughness;
        float denom = (NdotH * NdotH * (roughness2 - 1.0) + 1.0);
        float denom2 = denom * denom;
        return roughness2 / (PI * denom2);
}

float G_SchlickGGX(float NdotX, float roughness) {
        float k = (roughness * roughness) / 2.0;
        return NdotX / (NdotX * (1.0 - k) + k);
}

vec3 F_Schlick(float HdotV, vec3 F0) {
        return F0 + (1.0 - F0) * pow(1.0 - HdotV, 5.0);
}

vec3 F_Burley(float NdotL, float NdotV, float HdotV, float roughness, vec3 albedo) {
        float FD90 = 0.5 + 2.0 * roughness * HdotV * HdotV;
        float FL = 1.0 + (FD90 - 1.0) * pow(1.0 - NdotL, 5.0);
        float FV = 1.0 + (FD90 - 1.0) * pow(1.0 - NdotV, 5.0);
        return albedo * (FL * FV / PI);
}

vec3 ACESFilmic(vec3 x) {
        float a = 2.51;
        float b = 0.03;
        float c = 2.43;
        float d = 0.59;
        float e = 0.14;
        return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main() {
        vec4 albedoMetallic = texture(inAlbedo, fragUV);
        vec4 normalRoughness = texture(inNormal, fragUV);

        vec3 albedo = albedoMetallic.rgb;
        float metallic = albedoMetallic.a;

        vec3 normal = normalRoughness.rgb * 2.f - 1.f;
        float roughness = normalRoughness.a;

        vec3 emissive = texture(inEmissive, fragUV).rgb;

        float depth = texture(inDepth, fragUV).r;

        if (depth >= 1.0) {
                fragColor = vec4(0.0, 0.0, 0.0, 1.0);
                return;
        }

        vec4 clipPos = vec4(fragUV * 2.0 - 1.0, depth, 1.0);
        vec4 viewPos = ubo.invProj * clipPos;
        viewPos /= viewPos.w;
        vec4 worldPos = ubo.invView * viewPos;

        vec3 V = normalize(ubo.cameraPos - worldPos.xyz);
        vec3 H = normalize(lightDir + V);

        float NdotH = max(dot(normal, H), 0.0);
        float NdotV = max(dot(normal, V), 0.0);
        float NdotL = max(dot(normal, lightDir), 0.0);
        float HdotV = max(dot(H, V), 0.0);

        vec3 F0 = mix(vec3(0.04), albedo, metallic);

        float D = D_GGX(NdotH, roughness);
        float G = G_SchlickGGX(NdotV, roughness) * G_SchlickGGX(NdotL, roughness);
        vec3 F = F_Schlick(HdotV, F0);

        vec3 specular = (D * G * F) / max(4.0 * NdotV * NdotL, 0.001);
        vec3 kD = (1.0 - F) * (1.0 - metallic);
        vec3 diffuse = kD * F_Burley(NdotL, NdotV, HdotV, roughness, albedo);

        const float lightIntensity = 5.0;
        vec3 Lo = (diffuse + specular) * lightColor * lightIntensity * NdotL;
        vec3 ambient = albedo * 0.03;
        vec3 color = Lo + emissive + ambient;
        color = ACESFilmic(color);
        color = pow(color, vec3(1.0 / 2.2));
        fragColor = vec4(color, 1.0);
}