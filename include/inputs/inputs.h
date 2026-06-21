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
    enum class KeyCode {
        #include "keyDispatch.h"
        #include "keys.def"
        #include "keyDispatcherUndef.h"
    };

    struct InputContext {
        Scene &scene;
        float deltaTime = 0.0f;
    };

    using Key = std::set<std::pair<int, bool>>;
    using KeyCodeKey = std::set<std::pair<KeyCode, bool>>;
    using ActionToKeys = std::unordered_map<std::string, std::set<Key>>;
    using ActionToFunction = std::map<std::variant<std::string, std::pair<KeyCode, bool>>, std::function<void(const InputContext&)>>;

    struct BindingTable {
        uint32_t priority = 0;
        bool is_enabled = false;
        ActionToKeys actionToKeys = {};
        ActionToFunction actionToFunction = {};
        void addFunctionForAction(const std::string& action, const std::function<void(const InputContext&)>& function);
        void removeFunctionForAction(const std::string& action);
        void addAction(const std::string& action, Key keys);

        void addAction(const std::string &action, const KeyCodeKey& keys);

        void addAction(const std::string &action, KeyCode key, bool onlyOnPress);

        void removeAction(const std::string& action);
        void changeActionForFunction(const std::string& oldAction, const std::string& newAction);
        void changeFunctionForAction(const std::string& action, std::function<void(const InputContext&)> newFunction);
    };
private:
    GLFWwindow *window = nullptr;
    std::vector<BindingTable> bindingTables;
    std::unordered_map<int, bool> canPress;
public:
    void addBindingTable(const BindingTable& bindingTable);
    bool isDown(int key, bool onlyOnPress);
    bool isDown(const BindingTable &bindingTable, const std::string &action);
    bool isDown(KeyCode key, bool onlyOnPress);
    void InitInputs(GLFWwindow *_window);
    void handleInputs(const InputContext& context);
};

#endif //INPUTS_CLASS_H
