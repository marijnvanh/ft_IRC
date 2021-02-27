#ifndef __SERVER_H__
#define __SERVER_H__

#include <string>
#include <stdexcept>
#include <optional>
#include <queue>

#include "Client.h"
#include "IServer.h"
#include "IIOHandler.h"


class Server : public Client, IServer
{
    public:

    Server() = delete;
    Server(Client &&old_client) : Client(std::move(old_client)) {};
    ~Server() {};

    Server (Server& other) = delete;
    Server &operator =(Server& other) = delete;
    Server &operator= (Server&& other) = delete;

    private:
};

#endif
