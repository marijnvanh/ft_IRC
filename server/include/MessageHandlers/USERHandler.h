#ifndef USER_HANDLER_HPP
#define USER_HANDLER_HPP

#include "IClientDatabase.h"
#include "IMessage.h"
#include "IServerConfig.h"

auto USERHandler(IServerConfig *server_config, IClientDatabase *client_database, IMessage &message) -> void;

#endif