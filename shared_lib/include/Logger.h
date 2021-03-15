#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>
#include <mutex>
#include <unordered_map>

enum LogLevel
{
    DEBUG = 0,
    INFO,
    WARNING,
    ERROR,
    INVALID
};

class Logger
{
    public:

    Logger(std::string const &prefix);
    ~Logger();

    auto Log(LogLevel level, const char *format, ...) -> void;

    static auto SetAllLevels(LogLevel level) -> void;
    static auto SetLevel(std::string const &prefix, LogLevel const &level) -> void;
    static auto SetDefaultLogLevel(LogLevel level) -> void;
    static auto DisableLogs(bool enable) -> void; 

    private:
    static auto ParseLogLevel(LogLevel level) -> std::string;

    static std::mutex log_mutex_;
    static bool disable_logs_;
    static LogLevel default_level_;
    static std::unordered_map<std::string, LogLevel> log_settings_;

    std::string prefix_;
};

#endif
