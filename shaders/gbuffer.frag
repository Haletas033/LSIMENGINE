#version 450

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragUV;

layout(location = 0) out vec4 outAlbedo;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outEmissive;

void main() {
        vec3 albedo = vec3(1.f, 1.f, 1.f);
        float metallic = 0.0;

        vec3 normal = normalize(fragNormal);
        float roughness = 1.0;

        vec3 emissive = vec3(0.0);

        outAlbedo = vec4(albedo, metallic);
        outNormal = vec4(normal * 0.5 + 0.5, roughness);
        outEmissive = vec4(emissive, 1.0);
}