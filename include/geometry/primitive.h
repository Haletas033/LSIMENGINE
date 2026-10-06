#ifndef PRIMITIVES_CLASS_H
#define PRIMITIVES_CLASS_H

#include "meshData.h"

class Primitive {
public:
        enum Type {
                CUBE,
                PYRAMID,
                PLANE,
                SPHERE,
                TORUS,
                TERRAIN,
                MODEL
        };

        static MeshData GeneratePlane(float tileScale = 1.f);

        static MeshData GenerateCube(float tileScale = 1.f);

        static MeshData GeneratePyramid(float tileScale = 1.f);

        static MeshData GenerateSphere(int stacks = 30, int slices = 30, float tileScale = 1.f);

        static MeshData GenerateTorus(
                int ringSegments = 64,
                int tubeSegments = 32,
                float ringRadius = 0.35f,
                float tubeRadius = 0.18f,
                float tileScale = 1.f
        );
};

#endif //PRIMITIVES_CLASS_H
