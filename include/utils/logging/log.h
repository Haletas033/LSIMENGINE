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


#define STR_IMPL(x) #x
#define STR(x) STR_IMPL(x)

#define COL(NAME, CODE, RGB) constexpr auto NAME = STR(CODE##m);

namespace Ansi {
    constexpr auto START = "\033[";
    constexpr auto END = "\033[0m";
    #include "colorCodes.def"
}

#undef COL
#undef STR
#undef STR_IMPL

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

    static void InitEngineLogger();
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

extern Logger engineLogger;

#endif //LOG_H
