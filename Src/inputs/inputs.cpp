#include <include/inputs/inputs.h>
#include <include/utils/logging/log.h>

#include <algorithm>
#include <unordered_set>
#include <utility>
#include "include/utils/json.h"

extern nlohmann::ordered_json config;

static Logger logger;

void Inputs::InitInputs(GLFWwindow *_window) {
    logger = Logger("INPUTS");
    logger("stdInfo", "Successfully initialized the input loggers");
    window = _window;
}

void Inputs::addBindingTable(BindingTable& bindingTable) {
    bindingTables.insert(
        std::lower_bound(bindingTables.begin(), bindingTables.end(), bindingTable, [](const auto& a, const auto& b) {
            return a.get().priority > b.priority;
        }),
        std::ref(bindingTable)
    );
}

void Inputs::BindingTable::addFunctionForAction(const std::string& action, const std::function<void(const InputContext&)>& function) {
    if (actionToKeys.find(action) == actionToKeys.end()) {
        logger("stdWarning", "FAILED TO ADD FUNCTION TO ACTION \"" + action + "\": DOES NOT EXIST");
        return;
    }
    this->actionToFunction.insert({action, function});
}

void Inputs::BindingTable::removeFunctionForAction(const std::string &action) {
    this->actionToFunction.erase(action);
}

void Inputs::BindingTable::addAction(const std::string &action, Key keys) {
    if (actionToKeys.find(action) != actionToKeys.end()) {
        logger("stdWarning", "FAILED TO ADD ACTION \"" + action + "\": ALREADY EXISTS");
        return;
    }

    std::sort(keys.begin(), keys.end());

    this->actionToKeys.insert({action, {std::move(keys)}});
}

void Inputs::BindingTable::addAction(const std::string &action, const KeyCode key, KeyState state) {
    addAction(action, Key{{key, state}});
}

void Inputs::BindingTable::removeAction(const std::string &action) {
    removeFunctionForAction(action);
    this->actionToKeys.erase(action);
}

void Inputs::BindingTable::changeActionForFunction(const std::string &oldAction, const std::string &newAction) {
    if (actionToKeys.find(oldAction) == actionToKeys.end()) {
        logger("stdWarning", "FAILED TO CHANGE FUNCTION TO ACTION \"" + oldAction + "\": DOES NOT EXIST");
        return;
    }
    const auto oldFunction = this->actionToFunction.at(oldAction);
    removeFunctionForAction(oldAction);
    addFunctionForAction(newAction, oldFunction);
}

void Inputs::BindingTable::changeFunctionForAction(const std::string &action, std::function<void(const InputContext &)> newFunction) {
    if (actionToKeys.find(action) == actionToKeys.end()) {
        logger("stdWarning", "FAILED TO CHANGE FUNCTION TO ACTION \"" + action + "\": DOES NOT EXIST");
        return;
    }
    this->actionToFunction.at(action) = std::move(newFunction);
}

Inputs::KeyStateArray Inputs::poll() const {
    KeyStateArray keyStates{};

    for (const KeyCode key : validKeys) {
        keyStates[static_cast<size_t>(key)] = glfwGetKey(window, static_cast<int>(key)) == GLFW_PRESS;
    }

    return keyStates;
}

bool Inputs::isDown(const KeyCode key) const {
    return currentKeyStates[static_cast<size_t>(key)];
}

bool Inputs::justPressed(const KeyCode key) const {
    const auto index = static_cast<size_t>(key);
    return currentKeyStates[index] && !lastKeyStates[index];
}

bool Inputs::justReleased(const KeyCode key) const {
    const auto index = static_cast<size_t>(key);
    return !currentKeyStates[index] && lastKeyStates[index];
}

#define INPUT(func) bool Inputs::func(const BindingTable &bindingTable, const std::string& action) const { \
    auto& keys = bindingTable.actionToKeys.at(action); \
    for (const auto& key : keys) {\
        if (std::all_of(key.begin(), key.end(), [&](const auto &k) { return func(k.first); }))\
            return true;\
        }\
    return false;\
}\

INPUT(isDown)
INPUT(justPressed)
INPUT(justReleased)

#undef INPUT

void Inputs::handleInputs(const InputContext& context) {
    currentKeyStates = poll();

    std::unordered_set<KeyCode> consumed;
    for (auto& bindingTableRef : bindingTables) {
        BindingTable& bindingTable = bindingTableRef.get();

        if (!bindingTable.is_enabled) continue;
        for (const auto&[action, function] : bindingTable.actionToFunction) {
            auto &keys = bindingTable.actionToKeys.at(action);
            for (const auto &key: keys) {
                const bool blocked = std::ranges::any_of(key,
                [&](const auto& k) {
                        return consumed.contains(k.first);
                });

                if (blocked)
                    continue;

                if (std::ranges::all_of(key, [&](const auto &k) {
                    switch (k.second) {
                        case KeyState::JUST_PRESSED: return justPressed(k.first);
                        case KeyState::HELD: return isDown(k.first);
                        case KeyState::JUST_RELEASED: return justReleased(k.first);
                        default: return false;
                    }
                })) {
                    for (const auto &code: key | std::views::keys)
                        consumed.insert(code);

                    function(context);
                    break;
                }
            }

        }
    }

    lastKeyStates = currentKeyStates;
}