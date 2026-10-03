#ifndef LSIM_ENGINECONTEXT_H
#define LSIM_ENGINECONTEXT_H
#include "nlohmann/json.hpp"

struct Defaults;

namespace EngineContext {
        Defaults& getDefaults();
        const nlohmann::ordered_json& getConfig();
        const std::string& getWorkingDir();
}

#endif //LSIM_ENGINECONTEXT_H
