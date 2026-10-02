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

    static MeshData GeneratePlane(float tileScale);
    static MeshData GenerateCube(float tileScale);
    static MeshData GeneratePyramid(float tileScale);
    static MeshData GenerateSphere(int stacks, int slices, float tileScale);
    static MeshData GenerateTorus(int ringSegments, int tubeSegments, float ringRadius, float tubeRadius, float tileScale);
};

#endif //PRIMITIVES_CLASS_H
