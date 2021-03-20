#ifndef USER_HANDLER_HPP
#define USER_HANDLER_HPP

#include "IClientDatabase.h"
#include "IMessage.h"

auto USERHandler(IClientDatabase *client_database, IMessage &message) -> void;

#endif