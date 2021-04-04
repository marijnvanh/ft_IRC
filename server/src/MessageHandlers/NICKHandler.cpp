#include "MessageHandlers/NICKHandler.h"
#include "Numerics.h"
#include "Utilities.h"

#include "RemoteUser.h"

/* Valid NICK commands:

NICK from user:
    NICK <nickname>
        NICK new_nickname

NICK from server:
    NICK <nickname>
        :old_nick NICK new_nick

    NICK <nickname> <hopcount> <username> <host> <servertoken> <umode> <realname>
        NICK new_nick 1 username irc.codam.net 34 +i :Christophe Kalt
    
    ** We currently ignore host, servertoken and umode.
*/

#define PARAM_NICKNAME 0
#define PARAM_USERNAME 2
#define PARAM_SERVERNAME 3
#define PARAM_REALNAME 6

NICKHandler::NICKHandler(IServerConfig *server_config, IClientDatabase *client_database) :
    server_config_(server_config),
    client_database_(client_database),
    logger("NICKHandler")
{}

NICKHandler::~NICKHandler()
{}

auto NICKHandler::Handle(IMessage &message) -> void
{
    auto client = *(client_database_->GetClient(message.GetClientUUID()));

    auto params = message.GetParams();
    if (params.size() == 0)
    {
        client->Push(GetErrorMessage(ERR_NEEDMOREPARAMS));
        return ;
    }

    if (client->GetType() == IClient::Type::kLocalServer)
    {
        if (params.size() <= 2)
            HandleNicknameChangeFromServer(client, message);
        else if (params.size() >= 7)
            HandleNewRemoteUser(client, message);
        else
        {
            client->Push("ERROR: Unexpected amount of params for NICK command");
            return ;
        }
    }
    else
        HandleNICKFromUser(client, message);
}

auto NICKHandler::HandleNewRemoteUser(IClient* server, IMessage &message) -> void
{
    //TODO validate params
    auto new_nickname = message.GetParams()[PARAM_NICKNAME];
    auto new_username = message.GetParams()[PARAM_USERNAME];
    auto server_name = message.GetParams()[PARAM_SERVERNAME];
    auto new_realname = message.GetParams()[PARAM_REALNAME];
    
    auto client_with_same_nickname = client_database_->GetClient(new_nickname);
    if (client_with_same_nickname)
    {
        //TODO send kill command to both users with nickname
        return ;
    }
    auto remote_server = client_database_->GetServer(server_name);
    if (!remote_server)
    {
        server->Push(GetErrorMessage(ERR_NOSUCHSERVER, server_name));
        return ;
    }

    auto new_remote_user = std::make_unique<RemoteUser>(
        dynamic_cast<IServer*>(server),
        *remote_server,
        new_nickname,
        new_username,
        new_realname);
    client_database_->AddRemoteUser(std::move(new_remote_user));
}

auto NICKHandler::HandleNicknameChangeFromServer(IClient* server, IMessage &message) -> void
{
    //TODO validate nickname
    auto new_nickname = message.GetParams()[PARAM_NICKNAME];
    auto old_nickname = message.GetNickname();
    if (old_nickname == std::nullopt)
    {
        server->Push(GetErrorMessage(ERR_NONICKNAMEGIVEN));
        return ;
    }
    auto remote_user = client_database_->GetUser(*old_nickname);
    if (!remote_user)
    {
        server->Push(GetErrorMessage(ERR_NOSUCHNICK, *old_nickname));
        return ;
    }

    auto client_with_same_nickname = client_database_->GetClient(new_nickname);
    if (client_with_same_nickname)
    {
        //TODO send kill command to both users with nickname
        //Do after we finish kill command
        return ;
    }

    (*remote_user)->SetNickname(new_nickname);
}

auto NICKHandler::HandleNICKFromUser(IClient* client, IMessage &message) -> void
{
    //TODO validate nickname
    auto nickname = message.GetParams()[PARAM_NICKNAME];
    if (client->GetNickname() == nickname)
    {
        client->Push(GetErrorMessage(ERR_NICKCOLLISION));
        return ;
    }

    auto client_with_nickname = client_database_->GetClient(nickname);
    if (client_with_nickname)
    {
        client->Push(GetErrorMessage(ERR_NICKNAMEINUSE));
        return ;
    }

    client->SetNickname(nickname);

    if (client->GetType() != IClient::Type::kUnRegistered)
        ;//TODO Inform all connected clients that nickname has changed
    else if (client->GetType() == IClient::Type::kUnRegistered)
    {
        try {
            client = client_database_->RegisterLocalUser(client->GetUUID());
            auto welcome_message = ":" + server_config_->GetName() + " 001 " + 
                client->GetNickname() + " :Welcome to " + server_config_->GetDescription();
            client->Push(welcome_message);
        } catch (IClientDatabase::UnableToRegister &ex) {
            ;
        }
    }
}
