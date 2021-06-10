#include "MessageHandlers/QUITHandler.h"
#include "Numerics.h"
#include "Utilities.h"
#include "IServer.h"
#include "ILocalUser.h"
#include "IRemoteUser.h"

#define DEFAULT_QUIT_MESSAGE "Leaving"

QUITHandler::QUITHandler(IServerConfig *server_config, IClientDatabase *client_database) :
	CommandHandler(server_config, client_database, "QUIT", 0, true)
{}

QUITHandler::~QUITHandler()
{}

auto QUITHandler::SafeHandle(IMessage &message) -> void
{
    auto client = *(client_database_->GetClient(message.GetClientUUID()));

    if (client->GetType() == IClient::Type::kUnRegistered)
    {
        client_database_->DisconnectClient(client->GetUUID(), std::nullopt);
    }
    else if (client->GetType() == IClient::Type::kLocalUser)
    {
        DisconnectLocalUser(message);
    }
    else if (client->GetType() == IClient::Type::kLocalServer)
    {
        DisconnectRemoteUser(client, message);
    }
}

auto QUITHandler::GetQuitMessage(IMessage &message) -> std::string
{
    auto params = message.GetParams();
    if (params.size() < 1)
        return DEFAULT_QUIT_MESSAGE;
    else
        return params.front();
}

auto QUITHandler::DisconnectLocalUser(IMessage &message) -> void
{
    auto quit_message = GetQuitMessage(message);
    client_database_->DisconnectClient(message.GetClientUUID(),
		std::make_optional<std::string>(quit_message));
}

auto QUITHandler::DisconnectRemoteUser(IClient* server, IMessage &message) -> void
{
    auto nickname = message.GetNickname();
    if (nickname == std::nullopt)
    {
        server->Push(FormatERRORMessage(server->GetPrefix(), "QUIT No nickname given"));
        return ;
    }

    auto quit_message = GetQuitMessage(message);
    auto remote_client = client_database_->GetClient(*nickname);
    if (remote_client && (*remote_client)->GetType() == IClient::Type::kRemoteUser)
    {
        client_database_->DisconnectClient((*remote_client)->GetUUID(),
			std::make_optional<std::string>(quit_message));
    }
    else
    {
        server->Push(FormatERRORMessage(server->GetPrefix(), "QUIT No such nick: " + *nickname));
        return ;
    }
}
