#ifndef _ICLIENT_DATABASE_H__
#define _ICLIENT_DATABASE_H__

#include <unordered_map>
#include <functional>

#include "IClient.h"
#include "Mutex.h"
#include "UUID.h"

class IClientDatabase
{
    public:

    virtual ~IClientDatabase() {};

    virtual auto AddClient(std::unique_ptr<IClient> new_client) -> void = 0;
    virtual auto RemoveClient(IRC::UUID uuid) -> void = 0;
    virtual auto GetClient(IRC::UUID uuid) -> std::shared_ptr<IRC::Mutex<IClient>> = 0;

    class ClientNotFound : public std::runtime_error
    {
    public:
        ClientNotFound() : std::runtime_error("Client not found in Database") {}
    };

    class DuplicateClient : public std::runtime_error
    {
    public:
        DuplicateClient() : std::runtime_error("Client already in database") {}
    };
};

#endif
