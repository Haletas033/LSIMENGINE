#ifndef LSIM_UBO_H
#define LSIM_UBO_H

#include <glm/glm.hpp>

struct UBO {
        glm::mat4 model{1.0f};
        glm::mat4 view{1.0f};
        glm::mat4 proj{1.0f};
        glm::vec3 cameraPos{0.f, 0.f, 0.f};
        float _padding{};
};

#endif //LSIM_UBO_H
