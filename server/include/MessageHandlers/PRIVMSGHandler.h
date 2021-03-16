#ifndef PRIVMSG_HANDLER_HPP
#define PRIVMSG_HANDLER_HPP

#include "IMessage.h"
#include "IClientDatabase.h"

auto PRIVMSGHandler(IClientDatabase *client_database, IMessage &message) -> void;

#endif
