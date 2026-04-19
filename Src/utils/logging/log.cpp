//
// Created by halet on 9/7/2025.
//

#include <cstdarg>
#include <include/utils/logging/log.h>

#include "utils/json.h"

extern json config;

CapacityBuffer<Logger> Logger::logs(UINT16_MAX);

Logger::Logger(const std::string& subModule) {
    JSONManager::LoadLoggers(config, loggers);
    loggers["stdInfo"]->SetModule(subModule);
    loggers["stdWarn"]->SetModule(subModule);
    loggers["stdError"]->SetModule(subModule);
}

Logger& Logger::SetCustomTimeStamp(TimeFunc func) {
    this->useCustomTimeStamp = std::move(func);
    return *this;
}

Logger& Logger::HasTimeStamp() {
    this->hasTimeStamp = true;
    return *this;
}

Logger& Logger::SetColour(const std::string &c) { this->colour = c; return *this; }

Logger& Logger::SetType(const std::string &t) { this->type = t; return *this; }

Logger& Logger::SetModule(const std::string &m) { this->module = m; return *this; }

Logger& Logger::AddSubModules(const std::initializer_list<std::string>& sms) {
    for (const auto& sm : sms)
        this->subModules.push_back(sm);
    return *this;
}

Logger& Logger::AddSubModules(const std::vector<std::string>& sms) {
    for (const auto& sm : sms)
        this->subModules.push_back(sm);
    return *this;
}

std::string Logger::GetTimeStamp() {
    const time_t timestamp = time(nullptr);
    tm datetime;
    localtime_r(&timestamp, &datetime);

    char output[50];

    strftime(output, sizeof(output), "%H:%M:%S", &datetime);

    return {output};
}

std::string Logger::InsertBrackets(std::string field) {
    if (!field.empty() && field[0] != '[')
        return "[" + field + "]";
    return field;
}

std::string Logger::VectorToString(const std::vector<std::string>& items) {
    std::string output;
    for (const auto &item : items) {
        output += InsertBrackets(item);
    }
    return output;
}

Logger Logger::Temp() const {
    Logger clone = *this;
    return clone;
}

std::string Logger::GetLoggerMessage() const {
    const std::string localType = InsertBrackets(type);
    const std::string localModule = InsertBrackets(module);
    return timeStamp + localModule + VectorToString(subModules) + localType + message;
}

void Logger::operator()(const std::string& logger, const std::string &message) const {
    const auto it = loggers.find(logger);
    if (it == loggers.end()) {
        std::cerr << "Unable to find logger: " << logger << std::endl;
        return;
    }

    it->second->message = message;

    const std::string localType = InsertBrackets(it->second->type);
    const std::string localModule = InsertBrackets(it->second->module);

    if (it->second->hasTimeStamp) {
        if (it->second->useCustomTimeStamp)
            it->second->timeStamp = GetTimeStamp(it->second->useCustomTimeStamp);
        else
            it->second->timeStamp = GetTimeStamp();
        it->second->timeStamp = InsertBrackets(it->second->timeStamp);
    }

    logs.push_back(*it->second);

    std::cout << START << it->second->colour << it->second->timeStamp << localModule << VectorToString(it->second->subModules) << localType << message << END << std::endl;
}