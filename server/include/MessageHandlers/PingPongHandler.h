#ifndef PING_PONG_HANDLER_HPP
#define PING_PONG_HANDLER_HPP

#include <string>
#include <memory>

#include "ClientDatabase.h"

auto PingPongHandler(std::shared_ptr<ClientDatabase> client_database, std::string message) -> void;

#endif