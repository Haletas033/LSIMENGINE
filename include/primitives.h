#ifndef PRIMITIVES_CLASS_H
#define PRIMITIVES_CLASS_H

#include<vector>
#include<glad/glad.h>

#include "mesh.h"

class primitives {
public:
    Mesh GeneratePlane();
    Mesh GenerateCube();
    Mesh GeneratePyramid();
};

#endif //PRIMITIVES_CLASS_H
