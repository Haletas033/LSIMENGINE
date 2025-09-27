#include <include/geometry/primitives.h>
#include <include/geometry/mesh.h>

constexpr float PI = 3.14159265359f;

Mesh primitives::GeneratePlane(const float tileScale) {
    std::vector<GLfloat> vertices = {
        -0.5f, 0.0f, -0.5f,  0, 1, 0,  0, 0,
         0.5f, 0.0f, -0.5f,  0, 1, 0,  tileScale, 0,
         0.5f, 0.0f,  0.5f,  0, 1, 0,  tileScale, tileScale,
        -0.5f, 0.0f,  0.5f,  0, 1, 0,  0, tileScale,
    };

    std::vector<GLuint> indices = {
        0, 1, 2,
        2, 3, 0
    };
    return {vertices, indices};
}

Mesh primitives::GenerateCube(const float tileScale) {
    std::vector<GLfloat> vertices = {

        -0.5f, -0.5f,  0.5f,  0, 0, 1,  0, 0,
         0.5f, -0.5f,  0.5f,  0, 0, 1,  tileScale, 0,
         0.5f,  0.5f,  0.5f,  0, 0, 1,  tileScale, tileScale,
        -0.5f,  0.5f,  0.5f,  0, 0, 1,  0, tileScale,

        -0.5f, -0.5f, -0.5f,  0, 0, -1,  tileScale, 0,
        -0.5f,  0.5f, -0.5f,  0, 0, -1,  tileScale, tileScale,
         0.5f,  0.5f, -0.5f,  0, 0, -1,  0, tileScale,
         0.5f, -0.5f, -0.5f,  0, 0, -1,  0, 0,

        -0.5f, -0.5f, -0.5f, -1, 0, 0,  0, 0,
        -0.5f, -0.5f,  0.5f, -1, 0, 0,  tileScale, 0,
        -0.5f,  0.5f,  0.5f, -1, 0, 0,  tileScale, tileScale,
        -0.5f,  0.5f, -0.5f, -1, 0, 0,  0, tileScale,

         0.5f, -0.5f, -0.5f,  1, 0, 0,  tileScale, 0,
         0.5f,  0.5f, -0.5f,  1, 0, 0,  tileScale, tileScale,
         0.5f,  0.5f,  0.5f,  1, 0, 0,  0, tileScale,
         0.5f, -0.5f,  0.5f,  1, 0, 0,  0, 0,

        -0.5f, -0.5f, -0.5f,  0, -1, 0,  0, tileScale,
         0.5f, -0.5f, -0.5f,  0, -1, 0,  tileScale, tileScale,
         0.5f, -0.5f,  0.5f,  0, -1, 0,  tileScale, 0,
        -0.5f, -0.5f,  0.5f,  0, -1, 0,  0, 0,

        -0.5f,  0.5f, -0.5f,  0, 1, 0,  0, 0,
        -0.5f,  0.5f,  0.5f,  0, 1, 0,  0, tileScale,
         0.5f,  0.5f,  0.5f,  0, 1, 0,  tileScale, tileScale,
         0.5f,  0.5f, -0.5f,  0, 1, 0,  tileScale, 0
    };

    std::vector<GLuint> indices = {

        0, 1, 2,  2, 3, 0,

        4, 5, 6,  6, 7, 4,

        8, 9,10, 10,11, 8,

       12,13,14, 14,15,12,

       16,17,18, 18,19,16,

       20,21,22, 22,23,20
    };
    return {vertices, indices};
}

Mesh primitives::GeneratePyramid(const float tileScale) {
    // Vertices coordinates
    std::vector<GLfloat>vertices =
    { //     COORDINATES     /        COLORS      /   TexCoord  //
        -0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
        -0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	tileScale, 0.0f,
         0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
         0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	tileScale, 0.0f,
         0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	tileScale / 2, tileScale
    };

    // Indices for vertices order
    std::vector<GLuint> indices =
    {
        0, 1, 2,
        0, 2, 3,
        0, 1, 4,
        1, 2, 4,
        2, 3, 4,
        3, 0, 4
    };

    return {vertices, indices};
}

Mesh primitives::GenerateSphere(const int stacks = 20, const int slices = 30, const float tileScale = 1) {
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    for (int i = 0; i <= stacks; ++i) {
        float V = static_cast<float>(i) / stacks;
        const float phi = V * PI;

        for (int j = 0; j <= slices; ++j) {
            float U = static_cast<float>(j) / slices;
            const float theta = U * 2.0f * PI;

            float x = cosf(theta) * sinf(phi);
            float y = cosf(phi);
            float z = sinf(theta) * sinf(phi);

            // position
            vertices.push_back(x * 0.5f);
            vertices.push_back(y * 0.5f);
            vertices.push_back(z * 0.5f);

            // normal (same as position normalized)
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // texture coords
            vertices.push_back(U * tileScale);
            vertices.push_back(V * tileScale);
        }
    }

    // Indices
    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            const int first = (i * (slices + 1)) + j;
            const int second = first + slices + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    return {vertices, indices};
}

Mesh primitives::GenerateTorus(const int ringSegments = 30, const int tubeSegments = 20, const float ringRadius = 0.3f, const float tubeRadius = 0.1f, const float tileScale = 1) {
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    for (int i = 0; i <= ringSegments; ++i) {
        const float u = static_cast<float>(i) / ringSegments * 2.0f * PI;
        const float cosU = cosf(u);
        const float sinU = sinf(u);

        for (int j = 0; j <= tubeSegments; ++j) {
            const float v = static_cast<float>(j) / tubeSegments * 2.0f * PI;
            const float cosV = cosf(v);
            const float sinV = sinf(v);

            float x = (ringRadius + tubeRadius * cosV) * cosU;
            float y = tubeRadius * sinV;
            float z = (ringRadius + tubeRadius * cosV) * sinU;

            // position
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // normal (computed from parametric surface)
            float nx = cosU * cosV;
            float ny = sinV;
            float nz = sinU * cosV;
            vertices.push_back(nx);
            vertices.push_back(ny);
            vertices.push_back(nz);

            // texture coords
            vertices.push_back((static_cast<float>(i) / ringSegments) * tileScale);
            vertices.push_back((static_cast<float>(j) / tubeSegments) * tileScale);
        }
    }

    for (int i = 0; i < ringSegments; ++i) {
        for (int j = 0; j < tubeSegments; ++j) {
            const int first = i * (tubeSegments + 1) + j;
            const int second = first + tubeSegments + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    return {vertices, indices};
}