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

Defaults JSONManager::LoadConfigDefaults(json &config) {
    Defaults configDefaults;

    configDefaults.MAX_LIGHTS = config["shader-constants"]["MAX_LIGHTS"].get<unsigned int>();
    configDefaults.defaultWindowWidth = config["defaults"]["defaultWindowWidth"].get<unsigned int>();
    configDefaults.defaultWindowHeight = config["defaults"]["defaultWindowHeight"].get<unsigned int>();

    //Terrain defaults
    configDefaults.size        = config["defaults"]["size"].get<unsigned int>();
    configDefaults.gridScale   = config["defaults"]["gridScale"].get<float>();
    configDefaults.heightScale = config["defaults"]["heightScale"].get<float>();
    configDefaults.scale       = config["defaults"]["scale"].get<float>();
    configDefaults.octaves     = config["defaults"]["octaves"].get<int>();
    configDefaults.persistence = config["defaults"]["persistence"].get<float>();
    configDefaults.lacunarity  = config["defaults"]["lacunarity"].get<float>();


    //Sphere defaults
    configDefaults.sphereSlices = config["defaults"]["sphereSlices"].get<int>();
    configDefaults.sphereStacks = config["defaults"]["sphereStacks"].get<int>();

    //Torus defaults
    configDefaults.torusRingSegments = config["defaults"]["torusRingSegments"].get<int>();
    configDefaults.torusTubeSegments = config["defaults"]["torusTubeSegments"].get<int>();
    configDefaults.torusRingRadius   = config["defaults"]["torusRingRadius"].get<float>();
    configDefaults.torusTubeRadius   = config["defaults"]["torusTubeRadius"].get<float>();

    //Camera defaults
    configDefaults.FOVdeg    = config["defaults"]["FOVdeg"].get<float>();
    configDefaults.nearPlane = config["defaults"]["nearPlane"].get<float>();
    configDefaults.farPlane  = config["defaults"]["farPlane"].get<float>();

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

