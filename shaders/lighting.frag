#version 450

layout(set = 0, binding = 1) uniform sampler2D albedo;
layout(location = 0) in vec2 fragUV;
layout(location = 0) out vec4 fragColor;

void main() {
        fragColor = texture(albedo, fragUV);
}