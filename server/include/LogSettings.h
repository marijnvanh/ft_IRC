#ifndef __LOG_SETTINGS_H__
#define __LOG_SETTINGS_H__

#include "Logger.h"

auto SetLogSettings() -> void
{
    Logger::SetLevel("IRCServer", LogLevel::DEBUG);
    Logger::SetLevel("Client", LogLevel::INFO);
    Logger::SetLevel("MD", LogLevel::INFO); //MessageDisptacher
    Logger::SetLevel("ClientDatabase", LogLevel::DEBUG);
    Logger::SetLevel("ChannelDatabase", LogLevel::INFO);
    Logger::SetLevel("IOController", LogLevel::INFO);
}

#endif