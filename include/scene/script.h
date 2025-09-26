//
// Created by halet on 9/24/2025.
//

#ifndef SCRIPT_H
#define SCRIPT_H
#include <functional>
#include <vector>

#define REGISTER_SCRIPT(ScriptClass)                     \
class ScriptClass##Factory {                             \
public:                                                  \
ScriptClass##Factory() {                             \
Script::GetRegistry().emplace_back([]() -> Script* { return new ScriptClass(); }); \
}                                                    \
};                                                       \
static ScriptClass##Factory global_##ScriptClass##Factory;


class Script {
public:
    virtual ~Script() = default;

    virtual void Start() {};

    virtual void Update(const float deltaTime) {};

    using Creator = std::function<Script*()>;

    static std::vector<Creator>& GetRegistry() {
        static std::vector<Creator> registry;
        return registry;
    }

    static std::vector<Script*>& GetAllScripts() {
        static std::vector<Script*> scripts;
        return scripts;
    }

    static void InstantiateAll() {
        for (auto& creator : GetRegistry()) {
            GetAllScripts().push_back(creator());
        }
    }
};

#endif //SCRIPT_H
