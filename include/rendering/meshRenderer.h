#ifndef LSIM_MESHRENDERER_H
#define LSIM_MESHRENDERER_H

#include "geometry/meshPool.h"

struct MeshRenderer {
    MeshHandle meshHandle;
    MeshMode mode = MeshMode::STATIC;
};

#endif //LSIM_MESHRENDERER_H
