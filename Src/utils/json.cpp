#include <include/utils/json.h>

// Map for colour macros

using json = nlohmann::ordered_json;
std::unordered_map<std::string, const char*> JSONManager::colourMap = {
    {"RED", Ansi::RED},                         {"BLUE", Ansi::BLUE},
    {"BLACK", Ansi::BLACK},                     {"GREEN", Ansi::GREEN},
    {"YELLOW", Ansi::YELLOW},                   {"MAGENTA", Ansi::MAGENTA},
    {"CYAN", Ansi::CYAN},                       {"WHITE", Ansi::WHITE},
    {"BRIGHT_RED", Ansi::BRIGHT_RED},           {"BRIGHT_GREEN", Ansi::BRIGHT_GREEN},
    {"BRIGHT_YELLOW", Ansi::BRIGHT_YELLOW},     {"BRIGHT_BLUE", Ansi::BRIGHT_BLUE},
    {"BRIGHT_MAGENTA", Ansi::BRIGHT_MAGENTA},   {"BRIGHT_CYAN", Ansi::BRIGHT_CYAN},
    {"BRIGHT_WHITE", Ansi::BRIGHT_WHITE},       {"INFO_COLOUR", Ansi::INFO_COLOUR},
    {"WARNING_COLOUR", Ansi::WARNING_COLOUR},   {"ERROR_COLOUR", Ansi::ERROR_COLOUR}
};

void JSONManager::LoadJSON(const std::string &path, json &config) {
    std::ifstream file(path);
    if (!file.is_open()) throw std::runtime_error("Failed to open " + path);
    file >> config;
}

template <typename T>
void JSONManager::safeLoad(const nlohmann::json& json, const std::string& field, T &target) {
    if (json.contains(field)) {
        try {
            target = json[field].get<T>();
        } catch (const std::exception& e) {
            std::cerr << "Failed to load " << field << "using standard" << std::endl;
        }
    }
}

Defaults JSONManager::LoadConfigDefaults(json &config) {
    Defaults configDefaults;
    const json defaults = config["defaults"];

    configDefaults.MAX_LIGHTS = config["shader-constants"]["MAX_LIGHTS"].get<unsigned int>();
    safeLoad(defaults, "version", configDefaults.version);
    safeLoad(defaults, "defaultWindowWidth", configDefaults.defaultWindowWidth);
    safeLoad(defaults, "defaultWindowHeight", configDefaults.defaultWindowHeight);

    // Terrain defaults
    safeLoad(defaults, "size",        configDefaults.size);
    safeLoad(defaults, "gridScale",   configDefaults.gridScale);
    safeLoad(defaults, "heightScale", configDefaults.heightScale);
    safeLoad(defaults, "scale",       configDefaults.scale);
    safeLoad(defaults, "octaves",     configDefaults.octaves);
    safeLoad(defaults, "persistence", configDefaults.persistence);
    safeLoad(defaults, "lacunarity",  configDefaults.lacunarity);

    // Sphere defaults
    safeLoad(defaults, "sphereSlices", configDefaults.sphereSlices);
    safeLoad(defaults, "sphereStacks", configDefaults.sphereStacks);

    // Torus defaults
    safeLoad(defaults, "torusRingSegments", configDefaults.torusRingSegments);
    safeLoad(defaults, "torusTubeSegments", configDefaults.torusTubeSegments);
    safeLoad(defaults, "torusRingRadius",   configDefaults.torusRingRadius);
    safeLoad(defaults, "torusTubeRadius",   configDefaults.torusTubeRadius);

    // Camera defaults
    safeLoad(defaults, "FOVdeg",          configDefaults.FOVdeg);
    safeLoad(defaults, "nearPlane",       configDefaults.nearPlane);
    safeLoad(defaults, "farPlane",        configDefaults.farPlane);
    safeLoad(defaults, "sensitivity",     configDefaults.sensitivity);
    safeLoad(defaults, "speedMultiplier", configDefaults.speedMultiplier);

    // Input defaults
    safeLoad(defaults, "transformSpeed", configDefaults.transformSpeed);
    safeLoad(defaults, "rotationSpeed", configDefaults.rotationSpeed);

    return configDefaults;
}

std::string JSONManager::LoadShaderWithDefines(const std::string &path, json &config) {
    std::ifstream file(path);
    if (!file.is_open()) throw std::runtime_error("Failed to open " + path);
    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string defines = "#version 330 core\n";

    // Inject defines into the shader
    for (const auto&[name, value] : config["shader-constants"].items())
        defines += "#define " + name + " " + std::to_string(value.get<int>()) + "\n";

    return defines + buffer.str();
}

void JSONManager::LoadLoggers(json &config, std::unordered_map<std::string, std::shared_ptr<Logger>> &loggers) {
    auto loggersJson = config["loggers"];
    for (const auto &[loggerName, loggerValue] : loggersJson.items()) {
        auto logger = std::make_shared<Logger>();

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

        loggers[loggerName] = logger;
    }
}

Defaults JSONManager::InitJSON(const std::string &path, json &config) {
    LoadJSON(path, config);
    return LoadConfigDefaults(config);
}

