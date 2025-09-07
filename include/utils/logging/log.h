//
// Created by halet on 9/6/2025.
//

#ifndef LOG_H
#define LOG_H

#define START "\033["
#define END "\033[0m"

#define BLACK "30m"
#define RED "31m"
#define GREEN "32m"
#define YELLOW "33m"
#define BLUE "34m"
#define MAGENTA "35m"
#define CYAN "36m"
#define WHITE "37m"

#define BRIGHT_BLACK "90m"
#define BRIGHT_RED "91m"
#define BRIGHT_GREEN "92m"
#define BRIGHT_YELLOW "93m"
#define BRIGHT_BLUE "94m"
#define BRIGHT_MAGENTA "95m"
#define BRIGHT_CYAN "96m"
#define BRIGHT_WHITE "97m"

#define ERROR_COLOUR "31m"
#define WARNING_COLOUR "33m"
#define INFO_COLOUR "34m"

#include <functional>
#include <iostream>

class Logger {
private:

    using TimeFunc = std::function<std::string()>;
    TimeFunc useCustomTimeStamp;
    bool hasTimeStamp = false;

    std::string colour = BRIGHT_WHITE;
    std::string type;
    std::string module;
    std::vector<std::string> subModules;

    static std::string InsertBrackets(std::string field);

    static std::string VectorToString(const std::vector<std::string>& items);

    static std::string GetTimeStamp();

    template<typename TimeFunc>
    static std::string GetTimeStamp(TimeFunc func) {
        return func();
    }

public:
    Logger& SetCustomTimeStamp(TimeFunc func);

    Logger& HasTimeStamp();

    Logger& SetColour(const std::string &c);

    Logger& SetType(const std::string &t);

    Logger& SetModule(const std::string &m);

    Logger& AddSubModules(const std::initializer_list<std::string>& sms);

    Logger Temp() const;

    void operator()(const std::string &message);
};

#endif //LOG_H
