#ifndef LSIM_MATERIAL_H
#define LSIM_MATERIAL_H

#include <any>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <variant>

#include "glm/glm.hpp"
#include "resources/resourceManager.h"
#include "utils/logging/log.h"

using MaterialProperty = std::variant<int, float, glm::vec3, glm::vec4, glm::mat3, glm::mat4>;

enum class PropertyType {
        INT,
        FLOAT,
        VEC3,
        VEC4,
        MAT3,
        MAT4
};

class Material {
private:
        std::unordered_map<std::string, MaterialProperty> properties;
        std::string pipeline;

public:
        void addPipeline(const std::string& name);
        void addPipeline(const std::string& name, Pipeline&& pipelineTouse);
        void setPipeline(const std::string& name, Pipeline&& pipelineToUse);
        [[nodiscard]] const std::string& getPipeline() const { return pipeline; }
        void removePipeline() { pipeline = ""; }

        template <typename T>
        std::expected<void, LSIM::Error> setProperty(const std::string& name, T object) {
                if (!ResourceManager::getPipeline(pipeline)->getPushConstantOffsets().contains(name)) {
                        return std::unexpected(
                                LSIM::Error{
                                        LSIM::ErrorCode::LSIM_NO_PUSH_CONSTANT,
                                        LSIM::FatalityLevel::FATAL
                                }
                        );
                }
                properties[name] = object;
                return {};
        }

        [[nodiscard]] const std::unordered_map<std::string, MaterialProperty>& getProperties() const {
                return properties;
        }

        static Material createStandardPBR();
};

#endif //LSIM_MATERIAL_H
