#ifndef LSIM_SYSTEM_H
#define LSIM_SYSTEM_H
#include "registry.h"
#include "vk/context.h"

class System {
public:
        virtual void update(Context& context, Registry& registry, float deltaTime) = 0;
        virtual ~System() = default;
};

#endif //LSIM_SYSTEM_H
