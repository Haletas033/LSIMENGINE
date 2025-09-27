#ifndef PRIMITIVES_CLASS_H
#define PRIMITIVES_CLASS_H

#include<vector>
#include<glad/glad.h>

#include "mesh.h"

class primitives {
public:
    static Mesh GeneratePlane(float tileScale);
    static Mesh GenerateCube(float tileScale);
    static Mesh GeneratePyramid(float tileScale);
    static Mesh GenerateSphere(int stacks, int slices, float tileScale);
    static Mesh GenerateTorus(int ringSegments, int tubeSegments, float ringRadius, float tubeRadius, float tileScale);
};

#endif //PRIMITIVES_CLASS_H
