#include <rendering/material.h>

#include "utils/logging/log.h"

void Material::addTexture(const std::string &name, const std::string &path) {
    if (textures.contains(name)) {
        engineLogger("stdError", "\"" + name + "\" already exists.");
        return;
    }

    setTexture(name, path);
}

void Material::setTexture(const std::string &name, const std::string &path) {
    ResourceManager::addTexture(name, path);
    textures[name] = path;
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
}

void Material::setPipeline(const std::string &name, Pipeline&& pipelineToUse) {
    const auto& pipelines = ResourceManager::getPipelines();

    if (!pipelines.contains(name)) {
        ResourceManager::addPipeline(name, std::move(pipelineToUse));
    }

    pipeline = name;
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
    mat.addProperty("roughness", 0.5f);
    mat.addProperty("F0", 0.03f);
    mat.addProperty("emissiveIntensity", 0.f);
    mat.addProperty("meshColor", glm::vec4(1.f));
    return mat;
}
