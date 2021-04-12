#ifndef __ISERVER_H__
#define __ISERVER_H__

#include "IClient.h"
#include "IClientDatabase.h"

class IClientDatabase;

class IServer : public virtual IClient 
{
    public:

    virtual ~IServer() {};

    virtual auto AddClient(IClient* client) -> void = 0;
    virtual auto RemoveClient(IRC::UUID uuid) -> void = 0;
    virtual auto GetServerName() const -> const std::string& = 0;
    virtual auto Disconnect(IClientDatabase *client_database,
		std::optional<std::string> message) -> void = 0;

    private:
};

#endif
