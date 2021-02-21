#ifndef NICK_HANDLER_HPP
#define NICK_HANDLER_HPP

#include <string>
#include <memory>

#include "IClientDatabase.h"
#include "IClient.h"
#include "IMessage.h"
#include "Mutex.h"

auto NICKHandler(std::shared_ptr<IClientDatabase> client_database,
    std::shared_ptr<IRC::Mutex<IClient>> client, IMessage &message) -> void;

#endif