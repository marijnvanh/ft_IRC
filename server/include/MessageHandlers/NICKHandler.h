#ifndef NICK_HANDLER_HPP
#define NICK_HANDLER_HPP

#include "IMessage.h"
#include "IClientDatabase.h"

auto NICKHandler(IClientDatabase *client_database, IMessage &message) -> void;

#endif