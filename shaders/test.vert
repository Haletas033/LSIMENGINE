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

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position =
    ubo.proj *
    ubo.view *
    ubo.model *
    vec4(inPosition, 1.0);

    fragColor = inNormal;
}