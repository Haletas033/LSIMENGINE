#ifndef LSIM_REGISTRY_H
#define LSIM_REGISTRY_H
#include <any>
#include <assert.h>
#include <functional>
#include <optional>
#include <ranges>
#include <typeindex>
#include <unordered_map>
#include <utility>

#include "entityManager.h"
#include "ECS/componentTraits.h"

class SharedState;

class Registry {
private:
        EntityManager entityManager{};
        std::unordered_map<std::type_index, std::any> pools{};
        std::unordered_map<EntityHandle, std::vector<std::type_index>> componentTypes{};
        std::unordered_map<std::type_index, std::function<void(uint32_t)>> clearFuncs{};
        std::unordered_map<std::type_index, std::function<std::any(uint32_t)>> getComponentFuncs{};
        std::unordered_map<std::type_index, std::function<void(Registry&, SharedState, const std::any&)>> inspectFuncs{};

public:
        template<typename T>
        using Pool = std::vector<std::optional<std::decay_t<T>>>;

        template <typename T>
        void addComponent(const EntityHandle e, T component) {
                assert(isAlive(e));
                using Component = std::decay_t<T>;
                const auto id = std::type_index(typeid(Component));

                auto &slot = pools[id];
                componentTypes[e].emplace_back(id);
                if (!slot.has_value()) {
                        inspectFuncs[id] = ComponentTraits<T>::inspect;

                        slot = Pool<Component>{};

                        // Function used to destroy all of an entities components
                        clearFuncs[id] = [this, id](uint32_t index) {
                                auto &pool = std::any_cast<Pool<Component>&>(pools.at(id));
                                if (index < pool.size()) pool[index] = std::nullopt;
                        };

                        // Function used to get component using a runtime type
                        getComponentFuncs[id] = [this, id](uint32_t index) -> std::any {
                                auto &pool = std::any_cast<Pool<Component>&>(pools.at(id));
                                if (index < pool.size() && pool[index].has_value()) return pool[index].value();
                                return std::any{};
                        };
                }
                auto &pool = std::any_cast<Pool<Component>&>(slot);
                if (e.getIndex() >= pool.size()) pool.resize(e.getIndex()+1);
                pool[e.getIndex()].emplace(std::forward<T>(component));
        }

        template <typename T>
        void registerInspect(std::function<void(Registry&, SharedState, const std::any&)> fn) {
                inspectFuncs[std::type_index(typeid(T))] = std::move(fn);
        }

        std::function<void(Registry&, SharedState, const std::any&)> getInspectFunc(const std::type_index id) {
                return inspectFuncs[id];
        }

        template <typename T>
        bool hasComponent(const EntityHandle e) const {
                assert(isAlive(e));
                using Component = std::decay_t<T>;
                const auto id = std::type_index(typeid(Component));

                if (!pools.contains(id)) return false;
                auto &pool = std::any_cast<const Pool<Component>&>(pools.at(id));
                return pool.size() > e.getIndex() && pool[e.getIndex()];
        }

        template <typename T>
        std::decay_t<T>* getComponent(const EntityHandle e) {
                assert(isAlive(e));
                using Component = std::decay_t<T>;
                const auto id = std::type_index(typeid(Component));

                const auto it = pools.find(id);
                if (it == pools.end()) return nullptr;

                auto &pool = std::any_cast<Pool<Component>&>(it->second);

                if (e.getIndex() >= pool.size()) return nullptr;
                if (!pool[e.getIndex()]) return nullptr;

                return &*pool[e.getIndex()];
        }

        template <typename T>
        const std::decay_t<T>* getComponent(const EntityHandle e) const {
                assert(isAlive(e));
                using Component = std::decay_t<T>;
                const auto id = std::type_index(typeid(Component));

                const auto it = pools.find(id);
                if (it == pools.end()) return nullptr;

                auto &pool = std::any_cast<const Pool<Component>&>(it->second);

                if (e.getIndex() >= pool.size()) return nullptr;
                if (!pool[e.getIndex()]) return nullptr;

                return &*pool[e.getIndex()];
        }

        std::any getComponent(const EntityHandle e, const std::type_index type) const {
                assert(isAlive(e));
                if (const auto func = getComponentFuncs.find(type); func != getComponentFuncs.end()) {
                        return func->second(e.getIndex());
                }
                return std::any{};
        }

        std::vector<std::type_index> getComponentTypes(const EntityHandle e) const {
                assert(isAlive(e));
                if (const auto components = componentTypes.find(e); components != componentTypes.end()) {
                        return components->second;
                }
                return {};
        }

        template <typename T>
        void removeComponent(const EntityHandle e) {
                assert(isAlive(e));
                using Component = std::decay_t<T>;
                const auto id = std::type_index(typeid(Component));

                const auto it = pools.find(id);
                if (it == pools.end()) return;

                auto &pool = std::any_cast<Pool<Component>&>(it->second);
                componentTypes[e].erase(std::ranges::find(componentTypes[e], id));

                if (e.getIndex() >= pool.size()) return;
                if (!pool[e.getIndex()]) return;

                pool[e.getIndex()] = std::nullopt;
        }

        void destroyEntity(const EntityHandle e) {
                assert(isAlive(e));

                for (auto &func: clearFuncs | std::views::values) {
                        func(e.getIndex());
                }
                componentTypes.erase(e);
                entityManager.destroy(e);
        }

        EntityHandle create() { return entityManager.create(); }
        [[nodiscard]] bool isAlive(const EntityHandle e) const { return entityManager.isAlive(e); }
        [[nodiscard]] std::vector<EntityHandle> getAllAlive() const {
                return entityManager.getAllAlive();
        }

        static Registry& getDefaultRegistry() {
                static Registry instance;
                return instance;
        }
};

#endif //LSIM_REGISTRY_H
