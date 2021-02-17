#ifndef PASS_HANDLER_HPP
#define PASS_HANDLER_HPP

#include <string>
#include <memory>

#include "IClientDatabase.h"
#include "IMessage.h"

auto PASSHandler(std::shared_ptr<IRC::Mutex<IClient>> client, IMessage &message) -> void;

#endif