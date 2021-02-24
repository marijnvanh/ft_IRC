#ifndef KILL_HANDLER_HPP
#define KILL_HANDLER_HPP

#include <string>
#include <memory>

#include "Mutex.h"
#include "IClient.h"
#include "IMessage.h"
#include "IClientDatabase.h"

auto KILLHandler(std::shared_ptr<IClientDatabase> client_database, IMessage &message) -> void;

#endif
