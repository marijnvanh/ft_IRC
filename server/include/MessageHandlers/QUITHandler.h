#ifndef QUIT_HANDLER_HPP
#define QUIT_HANDLER_HPP

#include <string>
#include <memory>

#include "IClientDatabase.h"
#include "IMessage.h"

auto QUITHandler(IClientDatabase *client_database, IMessage &message) -> void;

#endif