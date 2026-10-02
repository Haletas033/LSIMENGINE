#ifndef LSIM_MESHRENDERER_H
#define LSIM_MESHRENDERER_H

#include "geometry/meshPool.h"

struct MeshRenderer {
    MeshHandle meshHandle;
    std::vector<float> vertices;
    std::vector<std::uint32_t> indices;
    MeshMode mode = MeshMode::STATIC;
};

#endif //LSIM_MESHRENDERER_H
