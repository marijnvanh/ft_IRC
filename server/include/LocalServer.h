#ifndef __LOCAL_SERVER_H__
#define __LOCAL_SERVER_H__

#include <string>
#include <stdexcept>
#include <optional>
#include <queue>

#include "Client.h"
#include "Server.h"
#include "ILocalServer.h"

class LocalServer : public Client, ILocalServer, Server
{
    public:

    LocalServer() = delete;
    LocalServer(std::string server_name, Client &&old_client);
    ~LocalServer();

    LocalServer (LocalServer& other) = delete;
    LocalServer &operator =(LocalServer& other) = delete;
    LocalServer &operator= (LocalServer&& other) = delete;

    private:
};

#endif
