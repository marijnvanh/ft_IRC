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
#define PARAM_HOPCOUNT 1
#define PARAM_USERNAME 2
#define PARAM_SERVERNAME 3
#define PARAM_SERV_TOKEN 4
#define PARAM_REALNAME 6

NICKHandler::NICKHandler(IServerConfig *server_config, IClientDatabase *client_database) :
    CommandHandler(server_config, client_database, "NICK", 1, true)
{}

NICKHandler::~NICKHandler()
{}

auto NICKHandler::SafeHandle(IMessage &message) -> void
{
    auto client = *(client_database_->GetClient(message.GetClientUUID()));
    auto params = message.GetParams();

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
    auto new_nickname = message.GetParams()[PARAM_NICKNAME];
	auto hopcount = atoi(message.GetParams()[PARAM_HOPCOUNT].c_str());
    auto new_username = message.GetParams()[PARAM_USERNAME];
    auto server_name = message.GetParams()[PARAM_SERVERNAME];
	auto server_token = atoi(message.GetParams()[PARAM_SERV_TOKEN].c_str());
    auto new_realname = message.GetParams()[PARAM_REALNAME];

    auto client_with_same_nickname = client_database_->GetClient(new_nickname);
    if (client_with_same_nickname)
    {
        HandleNickCollision(server, *client_with_same_nickname);
        return ;
    }

    auto remote_server = client_database_->GetServer(server_token);
    if (!remote_server)
    {
        server->Push(GetErrorMessage(server_config_->GetName(), ERR_NOSUCHSERVER, server_name));
        return ;
    }

    auto new_remote_user = std::make_unique<RemoteUser>(
        dynamic_cast<IServer*>(server),
        *remote_server,
        new_nickname,
        new_username,
        new_realname);

	new_remote_user->SetHops(hopcount);
	new_remote_user->SetTheirToken(server_token);
	new_remote_user->SetOurToken((*remote_server)->GetOurToken());

    auto nick_msg = new_remote_user->GenerateNickMessage(server_config_->GetName());

	client_database_->AddRemoteUser(std::move(new_remote_user));
    client_database_->BroadcastToLocalServers(nick_msg, server->GetUUID());
}

auto NICKHandler::HandleNicknameChangeFromServer(IClient* server, IMessage &message) -> void
{
    auto new_nickname = message.GetParams()[PARAM_NICKNAME];
    auto old_nickname = message.GetNickname();

    if (old_nickname == std::nullopt)
    {
        server->Push(GetErrorMessage(server_config_->GetName(), ERR_NONICKNAMEGIVEN));
        return ;
    }
    auto remote_user = client_database_->GetUser(*old_nickname);
    if (!remote_user)
    {
        server->Push(GetErrorMessage(server_config_->GetName(), ERR_NOSUCHNICK, *old_nickname));
        return ;
    }

    auto client_with_same_nickname = client_database_->GetClient(new_nickname);
    if (client_with_same_nickname)
    {
        HandleNickCollision(server, *client_with_same_nickname);
        return ;
    }

    (*remote_user)->SetNickname(new_nickname);
	(*remote_user)->CachePrefix((*remote_user)->GetRemoteServer()->GetServerName());
	
    auto nick_msg = ":" + *old_nickname + " NICK " + new_nickname;
    client_database_->BroadcastToLocalServers(nick_msg, server->GetUUID());
}

auto NICKHandler::HandleNICKFromUser(IClient* client, IMessage &message) -> void
{
    auto nickname = message.GetParams()[PARAM_NICKNAME];
    if (client->GetNickname() == nickname)
    {
        client->Push(GetErrorMessage(server_config_->GetName(), ERR_NICKCOLLISION));
        return ;
    }

    auto client_with_nickname = client_database_->GetClient(nickname);
    if (client_with_nickname)
    {
        client->Push(GetErrorMessage(server_config_->GetName(), ERR_NICKNAMEINUSE));
        return ;
    }

    std::string old_nickname = client->GetNickname();
	client->SetHops(0);
	client->SetOurToken(1);
    client->SetNickname(nickname);

    if (client->GetType() != IClient::Type::kUnRegistered)
    {
        auto nick_msg = ":" + old_nickname + " NICK " + nickname;
		dynamic_cast<IUser*>(client)->CachePrefix(server_config_->GetName());
        client_database_->BroadcastToLocalServers(nick_msg, std::nullopt);
    }
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

auto NICKHandler::HandleNickCollision(IClient* server, IClient* client_with_same_nickname) -> void
{
    auto nickname = client_with_same_nickname->GetNickname();
    auto kill_msg = ":" + server_config_->GetName() + " KILL " + nickname + " :Nick collision";
    server->Push(kill_msg);

    if (client_with_same_nickname->GetType() == IClient::Type::kUnRegistered ||
        client_with_same_nickname->GetType() == IClient::Type::kLocalUser)
    {
        client_database_->DisconnectClient(client_with_same_nickname->GetUUID(), "Nick collision");
    }
    else if (client_with_same_nickname->GetType() == IClient::Type::kRemoteUser) {
        client_with_same_nickname->Push(kill_msg);
    }
}
