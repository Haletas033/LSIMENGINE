#ifndef LSIM_REGISTRY_H
#define LSIM_REGISTRY_H
#include <any>
#include <assert.h>
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
        template<typename T>
        using Pool = std::vector<std::optional<std::decay_t<T>>>;

        template <typename T>
        void addComponent(const Entity e, T&& component) {
                assert(isAlive(e));
                using Component = std::decay_t<T>;
                const auto id = std::type_index(typeid(Component));

                auto &slot = pools[id];
                if (!slot.has_value()) {
                        slot = Pool<Component>{};

                        // Function used to destroy all of an entities components
                        clearFuncs[id] = [this, id](uint32_t index) {
                                auto &pool = std::any_cast<Pool<Component>&>(pools.at(id));
                                if (index < pool.size()) pool[index] = std::nullopt;
                        };
                }
                auto &pool = std::any_cast<Pool<Component>&>(slot);
                if (e.getIndex() >= pool.size()) pool.resize(e.getIndex()+1);
                pool[e.getIndex()] = std::forward<T>(component);
        }

        template <typename T>
        bool hasComponent(const Entity e) const {
                assert(isAlive(e));
                using Component = std::decay_t<T>;
                const auto id = std::type_index(typeid(Component));

                if (!pools.contains(id)) return false;
                auto &pool = std::any_cast<const Pool<Component>&>(pools.at(id));
                return pool.size() > e.getIndex() && pool[e.getIndex()];
        }

        template <typename T>
        std::decay_t<T>* getComponent(const Entity e) {
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
        const std::decay_t<T>* getComponent(const Entity e) const {
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

        template <typename T>
        void removeComponent(const Entity e) {
                assert(isAlive(e));
                using Component = std::decay_t<T>;
                const auto id = std::type_index(typeid(Component));

                const auto it = pools.find(id);
                if (it == pools.end()) return;

                auto &pool = std::any_cast<Pool<Component>&>(it->second);

                if (e.getIndex() >= pool.size()) return;
                if (!pool[e.getIndex()]) return;

                pool[e.getIndex()] = std::nullopt;
        }

        void destroyEntity(const Entity e) {
                assert(isAlive(e));

                for (auto &func: clearFuncs | std::views::values) {
                        func(e.getIndex());
                }
                entityManager.destroy(e);
        }

        Entity create() { return entityManager.create(); }
        [[nodiscard]] bool isAlive(const Entity e) const { return entityManager.isAlive(e); }
};

#endif //LSIM_REGISTRY_H
