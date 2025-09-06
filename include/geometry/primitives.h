#ifndef PRIMITIVES_CLASS_H
#define PRIMITIVES_CLASS_H

#include<vector>
#include<glad/glad.h>

#include "mesh.h"

class primitives {
public:
    static Mesh GeneratePlane();
    static Mesh GenerateCube();
    static Mesh GeneratePyramid();
    static Mesh GenerateSphere(int stacks, int slices);
    static Mesh GenerateTorus(int ringSegments, int tubeSegments, float ringRadius, float tubeRadius);
};

#endif //PRIMITIVES_CLASS_H
