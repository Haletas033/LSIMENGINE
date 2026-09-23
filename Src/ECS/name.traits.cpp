#include <ECS/name.traits.h>

void ComponentTraits<Name>::inspect(Registry& registry, SharedState &sharedState, const EntityHandle self) {
        const auto name = registry.getComponent<Name>(self);

        static char nameBuffer[128];
        strncpy(nameBuffer, name->value.c_str(), sizeof(nameBuffer));
        nameBuffer[sizeof(nameBuffer)-1] = '\0';

        if (ImGui::InputText("Name", nameBuffer, IM_ARRAYSIZE(nameBuffer))) {
                for (const auto e : sharedState.current_meshes()) {
                        if (!registry.hasComponent<Name>(e)) { continue; }
                        registry.getComponent<Name>(e)->value = nameBuffer;
                }
        }
}

std::vector<uint8_t> ComponentTraits<Name>::serialize(Registry& registry, const EntityHandle self) {
        const auto name = registry.getComponent<Name>(self);
        std::vector<uint8_t> data{};

        const auto append = [&data](const void* ptr, const size_t size) {
                const auto* bytes = static_cast<const uint8_t*>(ptr);
                data.insert(data.end(), bytes, bytes + size);
        };

        const uint32_t nameSize = name->value.size();
        append(&nameSize, sizeof(nameSize));
        append(name->value.data(), name->value.size());

        return data;
}

std::any ComponentTraits<Name>::deserialize(const std::vector<uint8_t> &data, size_t &ptr) {
        Name result{};

        const auto read = [&data, &ptr](void* dst, const size_t size) {
                if (ptr + size > data.size()) {
                        throw std::runtime_error("Invalid Transform data");
                }

                std::memcpy(dst, data.data() + ptr, size);
                ptr += size;
        };

        uint32_t size;
        read(&size, sizeof(size));

        if (size > data.size() - ptr) {
                throw std::runtime_error("Invalid Name string size");
        }

        result.value = std::string(
                reinterpret_cast<const char*>(data.data() + ptr),
                size
        );

        ptr += size;

        return result;
}