//
// Created by halet on 9/6/2025.
//

#ifndef LOG_H
#define LOG_H

#include <functional>
#include <iostream>
#include <memory>

#include "imgui.h"
#include "LSIMtypes.h"

namespace Ansi {
    constexpr auto START = "\033[";
    constexpr auto END = "\033[0m";
    constexpr auto BLACK = "30m";
    constexpr auto RED = "31m";
    constexpr auto GREEN = "32m";
    constexpr auto YELLOW = "33m";
    constexpr auto BLUE = "34m";
    constexpr auto MAGENTA = "35m";
    constexpr auto CYAN = "36m";
    constexpr auto WHITE = "37m";

    constexpr auto BRIGHT_BLACK = "90m";
    constexpr auto BRIGHT_RED = "91m";
    constexpr auto BRIGHT_GREEN = "92m";
    constexpr auto BRIGHT_YELLOW = "93m";
    constexpr auto BRIGHT_BLUE = "94m";
    constexpr auto BRIGHT_MAGENTA = "95m";
    constexpr auto BRIGHT_CYAN = "96m";
    constexpr auto BRIGHT_WHITE = "97m";

    constexpr auto ERROR_COLOUR = "31m";
    constexpr auto WARNING_COLOUR = "33m";
    constexpr auto INFO_COLOUR = "34m";
}

class Logger {
public:
    enum LogLevel {
        INFO,
        WARNING,
        ERR
    };
private:
    static CapacityBuffer<Logger> logs;

    LogLevel level = INFO;

    using TimeFunc = std::function<std::string()>;
    TimeFunc useCustomTimeStamp;
    bool hasTimeStamp = false;

    std::string timeStamp;

    std::string colour = Ansi::BRIGHT_WHITE;
    std::string type;
    std::string module;
    std::vector<std::string> subModules;

    std::string message;

    static std::string InsertBrackets(std::string field);

    static std::string VectorToString(const std::vector<std::string>& items);

    static std::string GetTimeStamp();

    template<typename TimeFunc>
    static std::string GetTimeStamp(TimeFunc func) {
        return func();
    }
public:
    std::unordered_map<std::string, std::shared_ptr<Logger>> loggers;

    static const CapacityBuffer<Logger>& GetLogs() { return logs; }

    explicit Logger(const std::string& subModule);

    Logger() = default;

    Logger& SetCustomTimeStamp(TimeFunc func);

    Logger& HasTimeStamp();

    Logger& SetColour(const std::string &c);

    Logger& SetType(const std::string &t);

    Logger& SetModule(const std::string &m);

    Logger& SetLevel(const LogLevel lev) { level = lev; return *this; }

    Logger& AddSubModules(const std::initializer_list<std::string>& sms);

    Logger& AddSubModules(const std::vector<std::string>& sms);

    std::string GetColour() const { return colour; }

    std::string GetType() const { return type; }

    std::string GetModule() const { return module; }

    LogLevel GetLevel() const { return level; }

    std::string GetLoggerMessage() const;

    Logger& operator[](const std::string& logger) const { return *this->loggers.at(logger); }

    void operator()(const std::string& logger, const std::string &message) const;
};

#endif //LOG_H
