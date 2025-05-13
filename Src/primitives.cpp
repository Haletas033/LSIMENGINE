#include"../include/primitives.h"

#include "../include/mesh.h"

Mesh primitives::GeneratePlane() {
    std::vector<GLfloat> vertices = {
        -0.5f, 0.0f, -0.5f,  0, 1, 0,  0, 0,
         0.5f, 0.0f, -0.5f,  0, 1, 0,  1, 0,
         0.5f, 0.0f,  0.5f,  0, 1, 0,  1, 1,
        -0.5f, 0.0f,  0.5f,  0, 1, 0,  0, 1,
    };

    std::vector<GLuint> indices = {
        0, 1, 2,
        2, 3, 0
    };
    return Mesh(vertices, indices);
}

Mesh primitives::GenerateCube() {
    std::vector<GLfloat> vertices = {

        -0.5f, -0.5f,  0.5f,  0, 0, 1,  0, 0,
         0.5f, -0.5f,  0.5f,  0, 0, 1,  1, 0,
         0.5f,  0.5f,  0.5f,  0, 0, 1,  1, 1,
        -0.5f,  0.5f,  0.5f,  0, 0, 1,  0, 1,

        -0.5f, -0.5f, -0.5f,  0, 0, -1,  1, 0,
        -0.5f,  0.5f, -0.5f,  0, 0, -1,  1, 1,
         0.5f,  0.5f, -0.5f,  0, 0, -1,  0, 1,
         0.5f, -0.5f, -0.5f,  0, 0, -1,  0, 0,

        -0.5f, -0.5f, -0.5f, -1, 0, 0,  0, 0,
        -0.5f, -0.5f,  0.5f, -1, 0, 0,  1, 0,
        -0.5f,  0.5f,  0.5f, -1, 0, 0,  1, 1,
        -0.5f,  0.5f, -0.5f, -1, 0, 0,  0, 1,

         0.5f, -0.5f, -0.5f,  1, 0, 0,  1, 0,
         0.5f,  0.5f, -0.5f,  1, 0, 0,  1, 1,
         0.5f,  0.5f,  0.5f,  1, 0, 0,  0, 1,
         0.5f, -0.5f,  0.5f,  1, 0, 0,  0, 0,

        -0.5f, -0.5f, -0.5f,  0, -1, 0,  0, 1,
         0.5f, -0.5f, -0.5f,  0, -1, 0,  1, 1,
         0.5f, -0.5f,  0.5f,  0, -1, 0,  1, 0,
        -0.5f, -0.5f,  0.5f,  0, -1, 0,  0, 0,

        -0.5f,  0.5f, -0.5f,  0, 1, 0,  0, 0,
        -0.5f,  0.5f,  0.5f,  0, 1, 0,  0, 1,
         0.5f,  0.5f,  0.5f,  0, 1, 0,  1, 1,
         0.5f,  0.5f, -0.5f,  0, 1, 0,  1, 0
    };

    std::vector<GLuint> indices = {

        0, 1, 2,  2, 3, 0,

        4, 5, 6,  6, 7, 4,

        8, 9,10, 10,11, 8,

       12,13,14, 14,15,12,

       16,17,18, 18,19,16,

       20,21,22, 22,23,20
    };
    return Mesh(vertices, indices);
}

Mesh primitives::GeneratePyramid() {
    // Vertices coordinates
    std::vector<GLfloat>vertices =
    { //     COORDINATES     /        COLORS      /   TexCoord  //
        -0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
        -0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
         0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
         0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
         0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	2.5f, 5.0f
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

    return Mesh(vertices, indices);
}