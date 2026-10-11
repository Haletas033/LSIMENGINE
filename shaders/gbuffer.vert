#version 450

layout(set = 0, binding = 0) uniform UBO {
        mat4 model;
        mat4 view;
        mat4 proj;
        vec3 cameraPos;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inTangent;
layout(location = 3) in vec2 inUV;

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragUV;

void main() {
        vec4 worldPosition = ubo.model * vec4(inPosition, 1.0);

        gl_Position = ubo.proj * ubo.view * worldPosition;

        mat3 normalMatrix = transpose(inverse(mat3(ubo.model)));
        fragNormal = normalize(normalMatrix * inNormal);
        fragUV = inUV;
}