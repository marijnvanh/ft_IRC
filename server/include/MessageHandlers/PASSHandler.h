#ifndef PASS_HANDLER_HPP
#define PASS_HANDLER_HPP

#include "IClientDatabase.h"
#include "IMessage.h"

auto PASSHandler(IClientDatabase *client_database, IMessage &message) -> void;

#endif