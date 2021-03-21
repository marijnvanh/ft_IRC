#ifndef NICK_HANDLER_HPP
#define NICK_HANDLER_HPP

#include "IClientDatabase.h"
#include "IMessage.h"
#include "IServerConfig.h"

auto NICKHandler(IServerConfig *server_config, IClientDatabase *client_database, IMessage &message) -> void;

#endif