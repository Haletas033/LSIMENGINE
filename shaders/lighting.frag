#version 450

layout(set = 0, binding = 1) uniform sampler2D inAlbedo;
layout(set = 0, binding = 2) uniform sampler2D inNormal;
layout(set = 0, binding = 3) uniform sampler2D inEmissive;

layout(location = 0) in vec2 fragUV;
layout(location = 0) out vec4 fragColor;

const vec3 lightDir = vec3(0, 1, 0);
const vec3 lightColor = vec3(1, 1, 1);

void main() {
        vec4 albedoMetallic = texture(inAlbedo, fragUV);
        vec4 normalRoughness = texture(inNormal, fragUV);

        vec3 albedo = albedoMetallic.rgb;
        float metallic = albedoMetallic.a;

        vec3 normal = normalRoughness.rgb * 2.f - 1.f;
        float roughness = normalRoughness.a;

        vec3 emissive = texture(inEmissive, fragUV).rgb;

        fragColor = vec4(max(dot(normal, lightDir), 0.0) * albedo * lightColor, 1.f);
}