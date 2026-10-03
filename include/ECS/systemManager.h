#ifndef LSIM_SYSTEMMANAGER_H
#define LSIM_SYSTEMMANAGER_H

#include <memory>
#include <typeindex>
#include <unordered_map>
#include <cassert>
#include <algorithm>
#include <queue>
#include <ranges>

#include "system.h"

enum class SystemStage {
        PRE_UPDATE,

        PRE_PHYSICS,
        PHYSICS,
        POST_PHYSICS,

        PRE_RENDER,
        RENDER,
        POST_RENDER,

        POST_UPDATE,

        STAGE_COUNT
};

struct SystemData {
        std::unique_ptr<System> system;
        SystemStage stage;
        bool isEnabled = true;
        std::vector<std::type_index> runsBefore{};
        std::vector<std::type_index> runsAfter{};
};

class SystemManager {
private:
        std::unordered_map<std::type_index, SystemData> systems{};
        std::array<std::vector<std::type_index>, static_cast<size_t>(SystemStage::STAGE_COUNT)> executionOrders{};
        bool isDirty = true;

        void rebuildExecutionOrders() {
                for (auto& order : executionOrders) {
                        order.clear();
                }

                for (int stageIdx = 0; stageIdx < static_cast<int>(SystemStage::STAGE_COUNT); ++stageIdx) {
                        const auto stage = static_cast<SystemStage>(stageIdx);
                        auto& executionOrder = executionOrders[stageIdx];

                        std::vector<std::pair<std::type_index, const SystemData*>> systemsAtStage{};
                        for (const auto &[type, data]: systems) {
                                if (data.stage == static_cast<SystemStage>(stage))
                                        systemsAtStage.emplace_back(type, &data);
                        }

                        std::unordered_map<std::type_index, std::vector<std::type_index>> edges;
                        std::unordered_map<std::type_index, int> inDegree;

                        for (const auto type: systemsAtStage | std::views::keys) {
                                inDegree[type] = 0;
                        }

                        for (const auto [type, data] : systemsAtStage) {
                                for (const auto entry : data->runsBefore) {
                                        if (inDegree.contains(entry)) {
                                                edges[type].push_back(entry);
                                                ++inDegree[entry];
                                        }
                                }

                                for (const auto entry : data->runsAfter) {
                                        if (inDegree.contains(entry)) {
                                                edges[entry].push_back(type);
                                                ++inDegree[type];
                                        }
                                }
                        }

                        std::queue<std::type_index> degreeZero;
                        for (const auto [type, degree] : inDegree) {
                                if (degree == 0) degreeZero.push(type);
                        }

                        while (!degreeZero.empty()) {
                                std::type_index type = degreeZero.front();
                                degreeZero.pop();
                                executionOrder.push_back(type);

                                for (const auto entry : edges[type]) {
                                        if ( --inDegree[entry] == 0) degreeZero.push(entry);
                                }
                        }

                        assert(executionOrder.size() == systemsAtStage.size() && "Cycle detected in system ordering");
                }

                isDirty = false;
        }
public:
        template <typename T>
        void addAtStage(const SystemStage stage) {
                SystemData systemData{
                        .system = std::make_unique<T>(),
                        .stage = stage,
                };
                auto [it, inserted] = systems.try_emplace(typeid(T), std::move(systemData));
                assert(inserted && "System already registered");
                isDirty = true;
        }

        template <typename T>
        void addAtStage(std::unique_ptr<T> system, const SystemStage stage) {
                SystemData systemData{
                        .system = std::move(system),
                        .stage = stage,
                };
                auto [it, inserted] = systems.try_emplace(typeid(T), std::move(systemData));
                assert(inserted && "System already registered");
                isDirty = true;
        }

        template <typename TSystem, typename TRelativeTo>
        void addBefore() {
                assert(systems.contains(typeid(TRelativeTo)) && "TRelativeTo not registered");

                if (!systems.contains(typeid(TSystem))) {
                        addAtStage<TSystem>(systems.at(typeid(TRelativeTo)).stage);
                }

                assert(std::ranges::find(systems.at(typeid(TSystem)).runsBefore, typeid(TRelativeTo))
                        == systems.at(typeid(TSystem)).runsBefore.end() && "Constraint already exists");

                systems.at(typeid(TSystem)).runsBefore.emplace_back(typeid(TRelativeTo));
                isDirty = true;
        }

        template <typename TSystem, typename TRelativeTo>
        void addBefore(std::unique_ptr<TSystem> system) {
                assert(systems.contains(typeid(TRelativeTo)) && "TRelativeTo not registered");

                if (!systems.contains(typeid(TSystem))) {
                        addAtStage<TSystem>(std::move(system), systems.at(typeid(TRelativeTo)).stage);
                }

                assert(std::ranges::find(systems.at(typeid(TSystem)).runsBefore, typeid(TRelativeTo))
                        == systems.at(typeid(TSystem)).runsBefore.end() && "Constraint already exists");

                systems.at(typeid(TSystem)).runsBefore.emplace_back(typeid(TRelativeTo));
                isDirty = true;
        }

        template <typename TSystem, typename TRelativeTo>
        void addAfter() {
                assert(systems.contains(typeid(TRelativeTo)) && "TRelativeTo not registered");

                if (!systems.contains(typeid(TSystem))) {
                        addAtStage<TSystem>(systems.at(typeid(TRelativeTo)).stage);
                }

                assert(std::ranges::find(systems.at(typeid(TSystem)).runsAfter, typeid(TRelativeTo))
                        == systems.at(typeid(TSystem)).runsAfter.end() && "Constraint already exists");

                systems.at(typeid(TSystem)).runsAfter.emplace_back(typeid(TRelativeTo));
                isDirty = true;
        }

        template <typename TSystem, typename TRelativeTo>
        void addAfter(std::unique_ptr<TSystem> system) {
                assert(systems.contains(typeid(TRelativeTo)) && "TRelativeTo not registered");

                if (!systems.contains(typeid(TSystem))) {
                        addAtStage<TSystem>(std::move(system), systems.at(typeid(TRelativeTo)).stage);
                }

                assert(std::ranges::find(systems.at(typeid(TSystem)).runsAfter, typeid(TRelativeTo))
                        == systems.at(typeid(TSystem)).runsAfter.end() && "Constraint already exists");

                systems.at(typeid(TSystem)).runsAfter.emplace_back(typeid(TRelativeTo));
                isDirty = true;
        }

        template <typename T>
        void enableSystem() {
                const auto it = systems.find(typeid(T));
                assert(it != systems.end() && "System not registered");
                it->second.isEnabled = true;
        }

        template <typename T>
        void disableSystem() {
                const auto it = systems.find(typeid(T));
                assert(it != systems.end() && "System not registered");
                it->second.isEnabled = false;
        }

        template <typename T>
        void removeSystem() {
                const auto it = systems.find(typeid(T));
                assert(it != systems.end() && "System not registered");
                systems.erase(it);

                for (auto &data: systems | std::views::values) {
                        std::erase(data.runsAfter, typeid(T));
                        std::erase(data.runsBefore, typeid(T));
                }

                isDirty = true;
        }

        template <typename T>
        bool hasSystem() const {
                return systems.contains(typeid(T));
        }

        template <typename T>
        bool isEnabled() const {
                assert(systems.contains(typeid(T)) && "System not registered");
                return systems.at(typeid(T)).isEnabled;
        }

        void update(
    const SystemStage stage,
    Registry& registry,
    const float deltaTime
) {
                if (isDirty) {
                        rebuildExecutionOrders();
                }

                const auto& executionOrder =
                    executionOrders[static_cast<size_t>(stage)];

                for (const auto type : executionOrder) {
                        auto& data = systems.at(type);

                        if (data.isEnabled) {
                                data.system->update(registry, deltaTime);
                        }
                }
        }

        static SystemManager& getDefaultSystemManager() {
                static SystemManager instance;
                return instance;
        }
};

#endif //LSIM_SYSTEMMANAGER_H
