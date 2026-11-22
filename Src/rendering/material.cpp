#include <rendering/material.h>

#include "utils/logging/log.h"

std::expected<TextureResource*, LSIM::Error> Material::addTexture(const std::string &name, const std::string &path, const VkFilter filter, const TextureType type) {
    if (textures.contains(name)) {
        engineLogger("stdError", "\"" + name + "\" already exists.");
        return std::unexpected(LSIM::Error{LSIM::ErrorCode::LSIM_TEXTURE_EXISTS, LSIM::FatalityLevel::FATAL});
    }

    auto tex = setTexture(name, path, filter, type);
    if (!tex) return std::unexpected(tex.error());
    return *tex;
}

std::expected<TextureResource*, LSIM::Error> Material::setTexture(const std::string &name, const std::string &path, const VkFilter filter, const TextureType type) {
    auto tex = ResourceManager::addTexture(name, path, filter, type);
    if (!tex) return std::unexpected(tex.error());
    textures[name] = path;
    return *tex;
}

void Material::addTexture(const std::string &name) {
    if (textures.contains(name)) {
        engineLogger("stdError", "\"" + name + "\" already exists.");
        return;
    }

    const auto &tex = ResourceManager::getTextures();
    if (const auto it = tex.find(name); it != tex.end()) {
        textures[name] = it->first;
    } else {
        engineLogger("stdError", "No texture named: \"" + name + "\" exists.");
    }
}

void Material::addPipeline(const std::string& name) {
    if (!pipeline.empty()) {
        engineLogger("stdError", "Material already has pipeline");
        return;
    }

    const auto &pipelines = ResourceManager::getPipelines();
    if (const auto it = pipelines.find(name); it != pipelines.end()) {
        pipeline = it->first;
    } else {
        engineLogger("stdError", "No pipeline named: \"" + name + "\" exists.");
    }

    for (const auto& property : ResourceManager::getPipeline(pipeline)->getPushConstantOffsets() | std::views::keys) {
        properties[property] = {};
    }
}

void Material::setPipeline(const std::string &name, Pipeline&& pipelineToUse) {
    const auto& pipelines = ResourceManager::getPipelines();

    if (!pipelines.contains(name)) {
        ResourceManager::addPipeline(name, std::move(pipelineToUse));
    }

    pipeline = name;

    for (const auto& property : ResourceManager::getPipeline(pipeline)->getPushConstantOffsets() | std::views::keys) {
        properties[property] = {};
    }
}

void Material::addPipeline(const std::string &name, Pipeline&& pipelineTouse) {
    if (!pipeline.empty()) {
        engineLogger("stdError", "Material already has a pipeline.");
        return;
    }

    setPipeline(name, std::move(pipelineTouse));
}

Material Material::createStandardPBR() {
    Material mat;
    mat.addPipeline("PBRShader");

    std::expected<void, LSIM::Error> res{};
    res = mat.setProperty("roughness", 0.5f);
    if (!res) throw std::runtime_error("PBRShader missing roughness");
    res = mat.setProperty("F0", 0.03f);
    if (!res) throw std::runtime_error("PBRShader missing F0");
    res = mat.setProperty("emissiveIntensity", 0.f);
    if (!res) throw std::runtime_error("PBRShader missing emissiveIntensity");
    res = mat.setProperty("meshColor", glm::vec4(1.f));
    if (!res) throw std::runtime_error("PBRShader missing meshColor");
    return mat;
}
