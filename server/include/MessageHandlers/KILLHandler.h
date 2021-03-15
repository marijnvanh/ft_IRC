#ifndef KILL_HANDLER_HPP
#define KILL_HANDLER_HPP

#include <string>
#include <memory>

#include "Mutex.h"
#include "IClient.h"
#include "IMessage.h"
#include "IClientDatabase.h"

auto KILLHandler(IClientDatabase *client_database, IMessage &message) -> void;

#endif
