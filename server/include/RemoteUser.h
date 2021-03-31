#ifndef __REMOTE_USER_H__
#define __REMOTE_USER_H__

#include "Client.h"
#include "User.h"
#include "IRemoteUser.h"
#include "IServer.h"

class RemoteUser : public IRemoteUser, User
{
    public:

    RemoteUser() = delete;
    RemoteUser(IServer *local_server, IServer *remote_server, std::string nickname, std::string username);
    ~RemoteUser();

    auto Push(std::string irc_message) -> void override;

    /* Fake IClient functionality */
    auto SendAll() -> void override;
    auto Receive() -> std::optional<std::string> override;

    private:

    Logger logger;
};

#endif
