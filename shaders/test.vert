#version 450

layout(location = 0) out vec3 fragColor;

vec2 positions[3] = vec2[](
        vec2( 0.0, -0.5),
        vec2(-0.5,  0.5),
        vec2( 0.5,  0.5)
);

vec3 fragColors[] = {
    vec3(1.f, 0.f, 0.f),
    vec3(0.f, 1.f, 0.f),
    vec3(0.f, 0.f, 1.f)
};

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    fragColor = fragColors[gl_VertexIndex];
}