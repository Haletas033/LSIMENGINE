#include "utils/serialization.h"

#include <cstring>

#include "ECS/registry.h"
#include "editor/sharedState.h"

static Logger logger;

void Serialization::InitSerialization() {
    logger = Logger("SERIALIZATION");
    logger("stdInfo", "Successfully initialized the serialization loggers");
}

void Serialization::saveToFile(std::ofstream &file, Registry& registry) {
    logger("stdInfo", "Beginning to write to file");

    const auto entities = registry.getAllAlive();

    // Number of entities
    const auto entityCount = static_cast<std::uint32_t>(entities.size());
    file.write(
        reinterpret_cast<const char*>(&entityCount),
        sizeof(entityCount)
    );

    for (const auto& e : entities) {
        // Entity marker
        constexpr std::string_view entityId = "engine.entity";

        constexpr auto entityIdSize = static_cast<std::uint32_t>(entityId.size());

        file.write(
            reinterpret_cast<const char*>(&entityIdSize),
            sizeof(entityIdSize)
        );

        file.write(
            entityId.data(),
            entityId.size()
        );

        for (const auto& type : registry.getComponentTypes(e)) {
            const auto& componentId = registry.getComponentId(type);
            const auto data = registry.serializeComponent(e, type);

            const auto idSize =
                static_cast<std::uint32_t>(componentId.size());

            file.write(
                reinterpret_cast<const char*>(&idSize),
                sizeof(idSize)
            );

            file.write(
                componentId.data(),
                static_cast<std::streamsize>(componentId.size())
            );

            if (!data.empty()) {
                file.write(
                    reinterpret_cast<const char*>(data.data()),
                    static_cast<std::streamsize>(data.size())
                );
            }
        }
    }

    logger("stdInfo", "Successfully wrote to file");
}

void Serialization::loadFromFile(std::ifstream &file, Registry &registry, SharedState& sharedState, const std::string &workingDir) {
    logger("stdInfo", "beginning to read from file");

    sharedState.set_current_entities(registry, {});
    for (const auto e : registry.getAllAlive()) {
        registry.destroyEntity(e);
    }

    const auto data = std::vector<uint8_t>(
        std::istreambuf_iterator(file),
        std::istreambuf_iterator<char>()
    );

    size_t ptr = 0;

    std::uint32_t entityCount;
    std::memcpy(&entityCount, data.data(), sizeof(entityCount));
    ptr += 4;

    std::uint32_t idSize;
    std::memcpy(&idSize, data.data() + ptr, sizeof(idSize));
    ptr += 4;

    std::string_view id(
        reinterpret_cast<const char*>(data.data() + ptr),
        idSize
    );
    ptr += idSize;

    if (id != "engine.entity") { throw std::runtime_error("EXPECTED engine.entity got " + std::string(id) + " instead"); }

    for (std::uint32_t i = 0; i < entityCount; ++i) {
        EntityHandle e = registry.create();

        while (true) {
            if (ptr == data.size()) {
                break;
            }

            std::memcpy(&idSize, data.data() + ptr, sizeof(idSize));

            ptr += 4;

            id = std::string_view(
                reinterpret_cast<const char*>(data.data() + ptr),
                idSize
            );
            ptr += idSize;

            if (id == "engine.entity") { break; }

            registry.deserializeComponent(e, id, ptr, data);
        }
    }

    logger("stdInfo", "Successfully read file");
}