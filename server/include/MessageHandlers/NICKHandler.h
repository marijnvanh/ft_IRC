#ifndef NICK_HANDLER_HPP
#define NICK_HANDLER_HPP

#include <string>
#include <memory>

#include "IClientDatabase.h"
#include "IClient.h"
#include "IMessage.h"
#include "Mutex.h"

auto NICKHandler(IClientDatabase *client_database, IMessage &message) -> void;

#endif