//
// Created by halet on 9/7/2025.
//

#include <include/utils/json.h>

// Map for colour macros
std::unordered_map<std::string, const char*> JSONManager::colourMap = {
    {"BLACK", BLACK}, {"RED", RED}, {"GREEN", GREEN},
    {"YELLOW", YELLOW}, {"BLUE", BLUE}, {"MAGENTA", MAGENTA},
    {"CYAN", CYAN}, {"WHITE", WHITE},
    {"BRIGHT_RED", BRIGHT_RED}, {"BRIGHT_GREEN", BRIGHT_GREEN},
    {"BRIGHT_YELLOW", BRIGHT_YELLOW}, {"BRIGHT_BLUE", BRIGHT_BLUE},
    {"BRIGHT_MAGENTA", BRIGHT_MAGENTA}, {"BRIGHT_CYAN", BRIGHT_CYAN},
    {"BRIGHT_WHITE", BRIGHT_WHITE}, {"INFO_COLOUR", INFO_COLOUR},
    {"WARNING_COLOUR", WARNING_COLOUR}, {"ERROR_COLOUR", ERROR_COLOUR}
};

void JSONManager::LoadJSON(const std::string &path, json &config) {
    std::ifstream file(path);
    if (!file.is_open()) throw std::runtime_error("Failed to open config.json");
    file >> config;
}

template <typename T>
void safeLoad(nlohmann::json json, const std::string field, T &target) {
    if (json.contains(field)) {
        try {
            target = json[field].get<T>();
        } catch (const std::exception e) {
            std::cerr << "Failed to load " << field << "using standard" << std::endl;
        }
    }
}

Defaults JSONManager::LoadConfigDefaults(json &config) {
    Defaults configDefaults;

    configDefaults.MAX_LIGHTS = config["shader-constants"]["MAX_LIGHTS"].get<unsigned int>();
    safeLoad(config["defaults"], "version", configDefaults.version);
    safeLoad(config["defaults"], "defaultWindowWidth", configDefaults.defaultWindowWidth);
    safeLoad(config["defaults"], "defaultWindowHeight", configDefaults.defaultWindowHeight);

    // Terrain defaults
    safeLoad(config["defaults"], "size",        configDefaults.size);
    safeLoad(config["defaults"], "gridScale",   configDefaults.gridScale);
    safeLoad(config["defaults"], "heightScale", configDefaults.heightScale);
    safeLoad(config["defaults"], "scale",       configDefaults.scale);
    safeLoad(config["defaults"], "octaves",     configDefaults.octaves);
    safeLoad(config["defaults"], "persistence", configDefaults.persistence);
    safeLoad(config["defaults"], "lacunarity",  configDefaults.lacunarity);

    // Sphere defaults
    safeLoad(config["defaults"], "sphereSlices", configDefaults.sphereSlices);
    safeLoad(config["defaults"], "sphereStacks", configDefaults.sphereStacks);

    // Torus defaults
    safeLoad(config["defaults"], "torusRingSegments", configDefaults.torusRingSegments);
    safeLoad(config["defaults"], "torusTubeSegments", configDefaults.torusTubeSegments);
    safeLoad(config["defaults"], "torusRingRadius",   configDefaults.torusRingRadius);
    safeLoad(config["defaults"], "torusTubeRadius",   configDefaults.torusTubeRadius);

    // Camera defaults
    safeLoad(config["defaults"], "FOVdeg",          configDefaults.FOVdeg);
    safeLoad(config["defaults"], "nearPlane",       configDefaults.nearPlane);
    safeLoad(config["defaults"], "farPlane",        configDefaults.farPlane);
    safeLoad(config["defaults"], "sensitivity",     configDefaults.sensitivity);
    safeLoad(config["defaults"], "speedMultiplier", configDefaults.speedMultiplier);

    // Input defaults
    safeLoad(config["defaults"], "transformSpeed", configDefaults.transformSpeed);


    return configDefaults;
}

std::string JSONManager::LoadShaderWithDefines(const std::string &path, json &config) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string defines = "#version 330 core\n";

    // Inject defines into the shader
    for (const auto&[name, value] : config["shader-constants"].items())
        defines += "#define " + name + " " + std::to_string(value.get<int>()) + "\n";

    return defines + buffer.str();
}

void JSONManager::LoadLoggers(json &config, std::unordered_map<std::string, std::unique_ptr<Logger>> &loggers) {
    auto loggersJson = config["loggers"];
    for (const auto &[loggerName, loggerValue] : loggersJson.items()) {
        auto logger = std::make_unique<Logger>();

        for (const auto &[fieldName, fieldValue] : loggerValue.items()) {
            if (fieldName == "hasTimeStamp") {
                logger->HasTimeStamp();
            } else if (fieldName == "colour") {
                // Check if the colour is a colour macro
                if (auto it = colourMap.find(fieldValue); it != colourMap.end()) {
                    logger->SetColour(it->second);
                } else {
                    logger->SetColour(fieldValue);
                }
            } else if (fieldName == "type") {
                logger->SetType(fieldValue);
            } else if (fieldName == "level") {
                logger->SetLevel(fieldValue);
            }
            else if (fieldName == "module") {
                logger->SetModule(fieldValue);
            } else if (fieldName == "sub-modules") {
                std::vector<std::string> sms;
                for (const auto &subModule : fieldValue) {
                    sms.push_back(subModule.get<std::string>());
                }
                logger->AddSubModules(sms);
            }
        }

        loggers[loggerName] = std::move(logger);
    }
}

Defaults JSONManager::InitJSON(const std::string &path, json &config,
    std::unordered_map<std::string, std::unique_ptr<Logger>> &loggers) {
    LoadJSON(path, config);
    LoadLoggers(config, loggers);
    return LoadConfigDefaults(config);
}

