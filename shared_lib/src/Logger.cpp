#include "Logger.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cstdarg>
#include <stdio.h>
#include <map>

#define MAX_LOG_LENGTH 1024

/* Init static members */
std::mutex Logger::log_mutex_;
bool Logger::disable_logs_ = false;
LogLevel Logger::default_level_ = LogLevel::ERROR;
std::unordered_map<std::string, LogLevel> Logger::log_settings_;

Logger::Logger(std::string const &prefix) : prefix_(prefix)
{
    if (log_settings_.find(prefix) == log_settings_.end())
    {
        SetLevel(prefix, default_level_);
    }
}

Logger::~Logger()
{}

static auto GetTimestamp() -> std::string
{
    const auto time = std::chrono::system_clock::now();
    const auto time_time_t = std::chrono::system_clock::to_time_t(time);
    const auto time_milli_sec = std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch()) % 1000;
    
    std::stringstream time_ss;
    time_ss << std::put_time(std::localtime(&time_time_t), "%T")
        << ':' << std::setfill('0') << std::setw(3) << time_milli_sec.count();
    return time_ss.str();
}

static char log_buffer[MAX_LOG_LENGTH];

auto Logger::Log(LogLevel level, const char *format, ...) -> void
{
    std::lock_guard<std::mutex> lock(log_mutex_);

    if (disable_logs_ || format == NULL || (log_settings_[prefix_] > level))
        return;

    va_list arg;
    va_start(arg, format);
    vsnprintf(log_buffer, sizeof(log_buffer), format, arg);
    va_end(arg);

    std::cout << GetTimestamp() << " " << ParseLogLevel(level) << " "
        << prefix_ << ": " << log_buffer << std::endl;
}

auto Logger::ParseLogLevel(LogLevel level) -> std::string
{
    static const std::string levels[4] = { "Debug", "Info", "Warning", "Error" };
    static const std::string colors[4] = { "[\e[37m", "[\e[35m", "[\e[33m", "[\e[31m" };

    if (level >= LogLevel::INVALID)
        return "ERROR FORMATTING LOG";
    return colors[level] + levels[level] + "\e[39m]";
}

auto Logger::ResolveLogLevel(const std::string &level) -> LogLevel
{
    static std::map<std::string, LogLevel> const log_levels = {
        {"DEBUG", LogLevel::DEBUG},
        {"INFO", LogLevel::INFO},
        {"WARNING", LogLevel::WARNING},
        {"ERROR", LogLevel::ERROR}
    };

    auto log_level = log_levels.find(level);

    if (log_level != log_levels.cend())
        return log_level->second;
    else
        return LogLevel::INVALID;
}

auto Logger::SetAllLevels(LogLevel level) -> void
{
    SetDefaultLogLevel(level);

    for (auto setting : log_settings_)
    {
		setting.second = level;
	}
}

auto Logger::SetLevel(std::string const &prefix, LogLevel const &level) -> void
{
    log_settings_[prefix] = level;
}

auto Logger::DisableLogs(bool enable) -> void
{
    disable_logs_ = enable;
}

auto Logger::SetDefaultLogLevel(LogLevel level) -> void
{
    default_level_ = level;
}
