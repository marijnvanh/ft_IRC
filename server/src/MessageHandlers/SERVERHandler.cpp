#include "Numerics.h"
#include "Utilities.h"
#include "MessageHandlers/SERVERHandler.h"
#include "RemoteServer.h"

#define PARAM_SERVER_NAME 0
#define PARAM_HOPCOUNT 1
#define PARAM_TOKEN 2
#define PARAM_INFO 3

SERVERHandler::SERVERHandler(IServerConfig *server_config, IClientDatabase *client_database, IChannelDatabase *channel_database) :
    CommandHandler(server_config, client_database, "SERVER", 2, true),
    channel_database_(channel_database)
{}

SERVERHandler::~SERVERHandler()
{}

auto SERVERHandler::SafeHandle(IMessage &message) -> void
{
	auto params = message.GetParams();
    auto client = *(client_database_->GetClient(message.GetClientUUID()));

	if (client->GetType() == IClient::Type::kLocalUser)
	{
		client->Push(GetErrorMessage(server_config_->GetName(), ERR_ALREADYREGISTERED));
		return;
	}
    
    /* Check if server already exists, if yes, disconnect current connection */
    auto server = client_database_->GetServer(params[PARAM_SERVER_NAME]);
    if (server)
    {
		client_database_->DisconnectClient(message.GetClientUUID(), std::nullopt);
        return ;
    }

    if (client->GetType() == IClient::Type::kLocalServer)
    {
        auto remote_server_name = message.GetServername();
        if (!remote_server_name)
        {
    		client->Push(GetErrorMessage(server_config_->GetName(), ERR_NEEDMOREPARAMS));
            return ;
        }
        auto remote_server = client_database_->GetServer(*remote_server_name);
        if (!remote_server)
        {
    		client->Push(GetErrorMessage(server_config_->GetName(), ERR_NOSUCHSERVER, *remote_server_name));
            return ;
        }
        auto local_server = (*client_database_->GetServer(client->GetUUID()));
        HandleRemoteServerRegistration(local_server, *remote_server, message);
        auto irc_message = ":" + *remote_server_name + " SERVER " +
			params[PARAM_SERVER_NAME] + " " +
			std::to_string(atoi(params[PARAM_HOPCOUNT].c_str()) + 1) +
			" :" + params[params.size() - 1];
        client_database_->BroadcastToLocalServers(irc_message, client->GetUUID());
    }
    else
    {
        HandleLocalServerRegistration(client, message);
    }
}

auto SERVERHandler::HandleLocalServerRegistration(IClient *client, IMessage &message) -> void
{
    auto new_server = client_database_->RegisterLocalServer(message.GetParams()[PARAM_SERVER_NAME], client->GetUUID());
    if (new_server->GetRegisterState() == IClient::RegisterState::kRegistering)
	{
		new_server->SetTheirToken(atoi(message.GetParams()[PARAM_HOPCOUNT].c_str()));
	}
    else
    {
		new_server->SetTheirToken(1);
		// TODO: Make sure to send the correct password to connect to remote.
		new_server->Push("PASS pass2 0210-IRC+ |");
        auto message = "SERVER " + server_config_->GetName() + " 1 :" +
			server_config_->GetDescription();
        new_server->Push(message);
    }
    new_server->SetRegisterState(IClient::RegisterState::kRegistered);
    HandleBroadcasting(new_server, message);
}

auto SERVERHandler::HandleRemoteServerRegistration(IServer *local_server, IServer *remote_server, IMessage &message) -> void
{
	auto params = message.GetParams();
    auto new_server = std::make_unique<RemoteServer>(local_server,
		remote_server, message.GetParams()[PARAM_SERVER_NAME]);
	
	new_server->SetHops(atoi(params[PARAM_HOPCOUNT].c_str()));
	new_server->SetTheirToken(atoi(params[PARAM_TOKEN].c_str()));

    client_database_->AddServer(std::move(new_server));
}

auto SERVERHandler::HandleBroadcasting(IClient *new_server, IMessage &message) -> void
{
    auto params = message.GetParams();

	std::string irc_message = ":" + server_config_->GetName() + " SERVER " +
		params[PARAM_SERVER_NAME] + " " +
		std::to_string((new_server->GetHops() + 1)) + " " +
		std::to_string(new_server->GetOurToken()) +
		" :" + params[params.size() - 1];
	
    client_database_->BroadcastToLocalServers(irc_message,
		std::make_optional<IRC::UUID>(message.GetClientUUID()));
    auto this_server_name = server_config_->GetName();
    client_database_->DoForEachServer(
        [this_server_name, new_server](IClient* client)
        {
            auto server = dynamic_cast<IServer*>(client);
            new_server->Push(server->GenerateServerMessage(this_server_name));
        }, std::make_optional<IRC::UUID>(new_server->GetUUID()));
    client_database_->DoForEachUser(
        [this_server_name, new_server](IClient* client)
        {
            auto user = dynamic_cast<IUser*>(client);
            new_server->Push(user->GenerateNickMessage(this_server_name));
        }, std::nullopt);
    channel_database_->ForEachChannel(
        [this_server_name, new_server](IChannel* channel)
        {
            auto njoin_msg = ":" + this_server_name + " NJOIN " + channel->GetName() +
                " :" + channel->GetUserListAsString(',');
            new_server->Push(njoin_msg);
        });

}
