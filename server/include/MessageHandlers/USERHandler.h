#ifndef USER_HANDLER_HPP
#define USER_HANDLER_HPP

#include <string>
#include <memory>

#include "IClientDatabase.h"
#include "IMessage.h"

auto USERHandler(std::shared_ptr<IClientDatabase> client_database, IMessage &message) -> void;

#endif