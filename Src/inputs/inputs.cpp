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

void Inputs::addBindingTable(BindingTable* bindingTable) {
    bindingTables.insert(
        std::lower_bound(bindingTables.begin(), bindingTables.end(), bindingTable, [](const BindingTable *a, const BindingTable *b) {
            return a->priority > b->priority;
        }),
        bindingTable
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
    this->actionToKeys.insert({action, std::set<Key>{std::move(keys)}});
}

void Inputs::BindingTable::addAction(const std::string &action, const KeyCodeKey& keys) {
    Key intKeys;
    for (const auto&[fst, snd] : keys)
        intKeys.insert({static_cast<int>(fst), snd});
    addAction(action, intKeys);
}

void Inputs::BindingTable::addAction(const std::string &action, const KeyCode key, const bool onlyOnPress) {
    addAction(action, KeyCodeKey{{key, onlyOnPress}});
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

bool Inputs::isDown(const int key, const bool onlyOnPress) {
    if (canPress.find(key) == canPress.end()) {
        canPress[key] = true;
    }

    if (glfwGetKey(window, key) == GLFW_PRESS) {
        if (onlyOnPress) {
            if (canPress[key]) {
                canPress[key] = false;
                return true;
            }
            return false;
        }
        return true;
    }

    if (glfwGetKey(window, key) == GLFW_RELEASE) {
        canPress[key] = true;
        return false;
    }
    return false;
}

bool Inputs::isDown(const KeyCode key, const bool onlyOnPress) {
    return isDown(static_cast<int>(key), onlyOnPress);
}

bool Inputs::isDown(const BindingTable &bindingTable, const std::string& action) {
    auto& keys = bindingTable.actionToKeys.at(action);
    for (const auto& key : keys) {
        if (std::all_of(key.begin(), key.end(), [&](const std::pair<int, bool> &k) { return isDown(k.first, k.second); }))
            return true;
    }
    return false;
}

void Inputs::handleInputs(const InputContext& context) {
    std::set<Key> consumed;
    for (BindingTable* bindingTable : bindingTables) {
        if (!bindingTable->is_enabled) continue;
        for (const auto&[action, function] : bindingTable->actionToFunction) {
            const auto& f = function;

            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    auto& keys = bindingTable->actionToKeys.at(arg);
                    for (const auto& key : keys) {
                        if (consumed.count(key) == 0) {
                            if (std::all_of(key.begin(), key.end(), [&](const std::pair<int, bool> &k) { return isDown(k.first, k.second); })) {
                                consumed.insert(keys.begin(), keys.end());
                                f(context);
                                break;
                            }
                        }
                    }
                } else if constexpr (std::is_same_v<T, std::pair<KeyCode, bool>>) {
                    int glfwKey = static_cast<int>(arg.first);
                    if (consumed.count(std::set{std::make_pair(glfwKey, arg.second)}) == 0) {
                        if (isDown(glfwKey, arg.second)) {
                            consumed.insert(std::set{std::make_pair(glfwKey, arg.second)});
                            f(context);
                        }
                    }
                }
            }, action);
        }
    }
}