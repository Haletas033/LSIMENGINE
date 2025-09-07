//
// Created by halet on 9/7/2025.
//

#ifndef JSON_H
#define JSON_H

#include <string>
#include <fstream>

#include "defaults.h"
#include "logging/log.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;
class JSONManager {
private:
    static std::unordered_map<std::string, const char*> colourMap;

    static void LoadJSON(const std::string &path, json &config);

    static Defaults LoadConfigDefaults(json &config);

    static void LoadLoggers(json &config, std::unordered_map<std::string, std::unique_ptr<Logger>> &loggers);
public:
    static Defaults InitJSON(const std::string &path, json &config,
        std::unordered_map<std::string, std::unique_ptr<Logger>> &loggers);

    static std::string LoadShaderWithDefines(const std::string &path, json &config);
};

#endif //JSON_H
