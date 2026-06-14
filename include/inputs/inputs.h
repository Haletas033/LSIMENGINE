#ifndef INPUTS_CLASS_H
#define INPUTS_CLASS_H

#include <memory>
#include <set>

#include "../scene/scene.h"
#include "../utils/fileIO.h"

#include <unordered_map>
#include <map>
#include <variant>
#include <GLFW/glfw3.h>
#include <vector>

#include "include/utils/defaults.h"



class Inputs {
public:
    using Key = std::set<std::pair<int, bool>>;
    enum KeyCode {
        A, B, C, D, E,
        F, G, H, I, J,
        K, L, M, N, O,
        P, Q, R, S, T,
        U, V, W, X, Y, Z
    };

    struct InputContext {
        Scene &scene;
        float deltaTime = 0.0f;
    };

    struct BindingTable {
        uint32_t priority = 0;
        bool is_enabled = false;
        std::unordered_map<std::string, std::set<Key>> actionToKeys;
        std::map<std::variant<std::string, std::pair<KeyCode, bool>>, std::function<void(const InputContext&)>> actionToFunction;
        void addFunctionForAction(const std::string& action, const std::function<void(const InputContext&)>& function);
        void removeFunctionForAction(const std::string& action);
        void addAction(const std::string& action, Key keys);
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
