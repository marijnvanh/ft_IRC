#ifndef REGISTER_USER_HPP
#define REGISTER_USER_HPP

#include <string>
#include <memory>

#include "IClientDatabase.h"
#include "IClient.h"
#include "Mutex.h"

auto RegisterUser(std::shared_ptr<IClientDatabase> client_database, std::shared_ptr<IRC::Mutex<IClient>> client) -> void;

#endif