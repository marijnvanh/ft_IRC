#ifndef PASS_HANDLER_HPP
#define PASS_HANDLER_HPP

#include <string>
#include <memory>

#include "IClient.h"
#include "IMessage.h"
#include "Mutex.h"

auto PASSHandler(IMessage &message) -> void;

#endif