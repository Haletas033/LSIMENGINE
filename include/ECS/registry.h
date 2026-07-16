#ifndef LSIM_REGISTRY_H
#define LSIM_REGISTRY_H
#include <any>
#include <optional>
#include <ranges>
#include <typeindex>
#include <unordered_map>

#include "entity.h"
#include "entityManager.h"


class Registry {
private:
        EntityManager entityManager{};
        std::unordered_map<std::type_index, std::any> pools{};
        std::unordered_map<std::type_index, std::function<void(uint32_t)>> clearFuncs{};

public:
        template <typename T>
        void addComponent(const Entity e, T component) {
                auto &slot = pools[typeid(T)];
                if (!slot.has_value()) {
                        slot = std::vector<std::optional<T>>{};

                        // Function used to destroy all of an entities components
                        clearFuncs[typeid(T)] = [this](uint32_t index) {
                                auto &pool = std::any_cast<std::vector<std::optional<T>>&>(pools.at(typeid(T)));
                                if (index < pool.size()) pool[index] = std::nullopt;
                        };
                }
                auto &pool = std::any_cast<std::vector<std::optional<T>>&>(slot);
                if (e.getIndex() >= pool.size()) pool.resize(e.getIndex()+1);
                pool[e.getIndex()] = component;
        }

        template <typename T>
        bool hasComponent(const Entity e) const {

                if (!pools.contains(typeid(T))) return false;
                auto &pool = std::any_cast<const std::vector<std::optional<T>>&>(pools.at(typeid(T)));
                return pool.size() > e.getIndex() && pool[e.getIndex()];
        }

        template <typename T>
        T* getComponent(const Entity e) {
                if (!hasComponent<T>(e)) return nullptr;
                return &std::any_cast<std::vector<std::optional<T>>&>(pools.at(typeid(T)))[e.getIndex()].value();
        }

        template <typename T>
        void removeComponent(const Entity e) {
                if (!hasComponent<T>(e)) return;
                std::any_cast<std::vector<std::optional<T>>&>(pools.at(typeid(T)))[e.getIndex()] = std::nullopt;
        }

        void destroyEntity(const Entity &e) {
                for (auto &func: clearFuncs | std::views::values) {
                        func(e.getIndex());
                }
                entityManager.destroy(e);
        }

        Entity create() { return entityManager.create(); }
        [[nodiscard]] bool isAlive(const Entity e) const { return entityManager.isAlive(e); }
};

#endif //LSIM_REGISTRY_H
