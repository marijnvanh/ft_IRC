#ifndef __SERVER_H__
#define __SERVER_H__

#include <string>
#include <stdexcept>
#include <optional>
#include <queue>

#include "Client.h"
#include "IServer.h"

class Server : public IServer, Client
{
    public:

    Server() = delete;
    Server(Client &&old_client);
    ~Server();

    auto AddClient(std::shared_ptr<IClient> client) -> void override;
    auto RemoveClient(IRC::UUID uuid) -> void override;

    Server (Server& other) = delete;
    Server &operator =(Server& other) = delete;
    Server &operator= (Server&& other) = delete;

    private:

    std::unordered_map<IRC::UUID, std::shared_ptr<IClient>> clients_;
};

#endif
