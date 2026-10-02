#ifndef LSIM_SYSTEM_H
#define LSIM_SYSTEM_H
#include "registry.h"

class System {
public:
        virtual void update(Registry& registry, float deltaTime) = 0;
        virtual ~System() = default;
};

#endif //LSIM_SYSTEM_H
