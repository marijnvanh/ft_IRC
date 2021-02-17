#ifndef PASS_HANDLER_HPP
#define PASS_HANDLER_HPP

#include <string>
#include <memory>

#include "IClient.h"
#include "IMessage.h"
#include "Mutex.h"

auto PASSHandler(std::shared_ptr<IRC::Mutex<IClient>> client, IMessage &message) -> void;

#endif