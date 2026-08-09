#ifndef LSIM_SYSTEM_H
#define LSIM_SYSTEM_H
#include "registry.h"

class System {
public:
        virtual void update(Registry& registry, float deltaTime);
        virtual ~System();
};

#endif //LSIM_SYSTEM_H
