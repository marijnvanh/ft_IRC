#ifndef USER_HANDLER_HPP
#define USER_HANDLER_HPP

#include <string>
#include <memory>

#include "IClientDatabase.h"
#include "IClient.h"
#include "IMessage.h"
#include "Mutex.h"

auto USERHandler(std::shared_ptr<IClientDatabase> client_database, IMessage &message) -> void;

// Only after both USER and NICK have been received a user is registered
// Before we handle USER we need to have a nickname.

// USER message from a client:
// Example: USER guest tolmoon tolsun :Ronnie Reagan
// Not prefixed with nickname
// hostname and server are ignored
// Set username and state in Client and let all connected servers know about new user

// USER message (forwarded) from a server example:
// :testnick USER guest tolmoon tolsun :Ronnie Reagan
// Prefixed with nickname
// Save new Client in client database including hostname and servername
// Forward USER message to all other servers ?

// realname is the last parameter, prefixed with ':' so it can contain spaces

// Errors:
// ERR_NEEDMOREPARAMS
// ERR_ALREADYREGISTRED

#endif