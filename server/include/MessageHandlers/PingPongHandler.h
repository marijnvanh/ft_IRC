#ifndef PING_PONG_HANDLER_HPP
#define PING_PONG_HANDLER_HPP

#include <string>
#include <memory>

#include "IClientDatabase.h"
#include "Message.h"

auto PingPongHandler(IClientDatabase *client_database, Message message) -> void;

#endif