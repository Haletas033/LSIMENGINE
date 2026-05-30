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

class JSONManager {
private:
    static std::unordered_map<std::string, const char*> colourMap;

    template<class T>
    static void safeLoad(const nlohmann::json &json, const std::string &field, T &target);
public:
    static void LoadJSON(const std::string &path, nlohmann::ordered_json &config);

    static Defaults LoadConfigDefaults(nlohmann::ordered_json &config);

    static void LoadLoggers(nlohmann::ordered_json &config, std::unordered_map<std::string, std::shared_ptr<Logger>> &loggers);

    static Defaults InitJSON(const std::string &path, nlohmann::ordered_json &config);

    static std::string LoadShaderWithDefines(const std::string &path, nlohmann::ordered_json &config);
};

#endif //JSON_H
