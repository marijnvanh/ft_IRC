#ifndef PING_PONG_HANDLER_HPP
#define PING_PONG_HANDLER_HPP

#include <string>
#include <memory>

#include "ClientDatabase.h"
#include "Message.h"

auto PingPongHandler(std::shared_ptr<ClientDatabase> client_database, Message message) -> void;

#endif