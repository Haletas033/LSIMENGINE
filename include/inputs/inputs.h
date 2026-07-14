#ifndef INPUTS_CLASS_H
#define INPUTS_CLASS_H

#include <functional>
#include <memory>
#include <set>

#include "../scene/scene.h"

#include <unordered_map>
#include <map>
#include <variant>
#include <GLFW/glfw3.h>
#include <vector>

#include "include/utils/defaults.h"

class Inputs {
public:
    #include "keyDispatch.h"
    enum class KeyCode {
        #undef DELETE // Needed on Windows to avoid collision
        #define K1(key) key = GLFW_KEY_##key,
        #define K2(name, key) name = GLFW_KEY_##key,

        #include "keys.def"

        #undef K1
        #undef K2
    };

    static constexpr KeyCode validKeys[] = {
        #define K1(key) KeyCode::key,
        #define K2(name, key) KeyCode::name,

        #include "keys.def"

        #undef K1
        #undef K2
    };
    #include "keyDispatcherUndef.h"

    enum class KeyState {
        JUST_PRESSED,
        HELD,
        JUST_RELEASED
    };

    struct InputContext {
        Scene &scene;
        float deltaTime = 0.0f;
    };

    using Key = std::vector<std::pair<KeyCode, KeyState>>;
    using ActionToKeys = std::unordered_map<std::string, std::vector<Key>>;
    using ActionToFunction = std::unordered_map<std::string, std::function<void(const InputContext&)>>;
    using KeyStateArray = std::array<bool, GLFW_KEY_LAST + 1>;

    struct KeyHash {
        std::size_t operator()(const Key& key) const noexcept {
            std::size_t hash = 0;

            for (const auto& [code, state] : key) {
                hash ^= std::hash<int>{}(static_cast<int>(code))
                        + 0x9e3779b9
                        + (hash << 6)
                        + (hash >> 2);

                hash ^= std::hash<int>{}(static_cast<int>(state))
                        + 0x9e3779b9
                        + (hash << 6)
                        + (hash >> 2);
            }

            return hash;
        }
    };

    struct BindingTable {
        uint32_t priority = 0;
        bool is_enabled = false;
        ActionToKeys actionToKeys = {};
        ActionToFunction actionToFunction = {};
        void addFunctionForAction(const std::string& action, const std::function<void(const InputContext&)>& function);
        void removeFunctionForAction(const std::string& action);
        void addAction(const std::string& action, Key keys);

        void addAction(const std::string &action, KeyCode key, KeyState state);

        void removeAction(const std::string& action);
        void changeActionForFunction(const std::string& oldAction, const std::string& newAction);
        void changeFunctionForAction(const std::string& action, std::function<void(const InputContext&)> newFunction);
    };
private:
    GLFWwindow *window = nullptr;
    std::vector<std::reference_wrapper<BindingTable>> bindingTables;
    KeyStateArray currentKeyStates{};
    KeyStateArray lastKeyStates{};
public:
    KeyStateArray poll() const;
    void addBindingTable(BindingTable &bindingTable);

    bool isDown(KeyCode key) const;
    bool justPressed(KeyCode key) const;
    bool justReleased(KeyCode key) const;

    #define INPUT(func) bool func(const BindingTable &bindingTable, const std::string &action) const;

    INPUT(isDown)
    INPUT(justPressed)
    INPUT(justReleased)

    #undef INPUT

    void InitInputs(GLFWwindow *_window);
    void handleInputs(const InputContext& context);
};

#endif //INPUTS_CLASS_H
