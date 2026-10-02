#ifndef LSIM_NAMESYSTEM_H
#define LSIM_NAMESYSTEM_H
#include "name.h"
#include "system.h"
#include "inputs/gui.h"

class NameSystem final : public System {
private:
        std::unordered_map<EntityHandle, Gui::Node*> nodes{};

public:
        void update(Registry &registry, float deltaTime) override {
                for (const EntityHandle e : registry.getAllAlive()) {
                        if (!registry.hasComponent<Name>(e)) continue;
                        if (nodes.contains(e)) continue;

                        auto* node = new Gui::Node{ e, Gui::root, {} };
                        Gui::root->children.push_back(node);
                        nodes[e] = node;
                }

                for (auto it = nodes.begin(); it != nodes.end();) {
                        if (!registry.isAlive(it->first) || !registry.hasComponent<Name>(it->first)) {
                                Gui::DeleteNodeRecursively(it->second);
                                it = nodes.erase(it);
                        } else {
                                ++it;
                        }
                }
        }
};

#endif //LSIM_NAMESYSTEM_H
