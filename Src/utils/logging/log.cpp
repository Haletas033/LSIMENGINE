//
// Created by halet on 9/7/2025.
//

#include <cstdarg>
#include <include/utils/logging/log.h>

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

std::string Logger::GetTimeStamp() {
    const time_t timestamp = time(nullptr);
    const tm datetime = *localtime(&timestamp);

    static char output[50];

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


void Logger::operator()(const std::string &message) {
    this->type = InsertBrackets(type);
    this->module = InsertBrackets(module);

    std::string timeStamp;

    if (this->hasTimeStamp) {
        if (this->useCustomTimeStamp) {
            timeStamp = GetTimeStamp(this->useCustomTimeStamp);
        } else {
            timeStamp = GetTimeStamp();
        }
        timeStamp = InsertBrackets(timeStamp);
    }

    std::cout << START << this->colour << timeStamp << this->module << VectorToString(this->subModules) << this->type << message << END << std::endl;
}