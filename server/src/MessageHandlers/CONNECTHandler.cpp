#include "Numerics.h"
#include "Utilities.h"
#include "MessageHandlers/CONNECTHandler.h"

#define SERVER_NAME_PARAM 0

CONNECTHandler::CONNECTHandler(IServerConfig *server_config,
                            IClientDatabase *client_database,
                            IRCServer *irc_server) :
    client_database_(client_database),
    server_config_(server_config),
    irc_server_(irc_server),
    logger("CONNECTHandler")
{}

CONNECTHandler::~CONNECTHandler()
{}

auto CONNECTHandler::Handle(IMessage &message) -> void
{
	auto params = message.GetParams();
    auto client = *(client_database_->GetClient(message.GetClientUUID()));

	// Handle unregistered client.
	if (client->GetType() == IClient::Type::kUnRegistered)
	{
		client->Push(GetErrorMessage(ERR_NOTREGISTERED));
		return;
	}	
	// Handle not enough parameters.
	if (params.size() == 0)
	{
		client->Push(GetErrorMessage(ERR_NEEDMOREPARAMS, "CONNECT"));
		return;
	}

	if (client->GetType() == IClient::Type::kLocalServer)
	{
		client->Push("ERROR :CONNECT from server not supported");
        return ;
    }
	auto user = dynamic_cast<IUser*>(client);
	if (!user->HasMode(UserMode::UM_OPERATOR))
	{
		client->Push(GetErrorMessage(ERR_NOPRIVILEGES, "CONNECT"));
		return ;
	}
	auto authorized_servers = server_config_->GetAuthorizedServers();
    //TODO validate servername
    auto server = authorized_servers.find(params[SERVER_NAME_PARAM]);
    if (server == authorized_servers.end())
	{
		client->Push(GetErrorMessage(ERR_NOSUCHSERVER, "CONNECT")); // Not an authorized server
        return ;
    }

    auto new_server = irc_server_->CreateNewConnection(server->second.first, server->second.second);
	if (!new_server)
	{
		logger.Log(LogLevel::WARNING, "Failed to connect to server: %s", params[SERVER_NAME_PARAM].c_str());
        return ;
    }
	(*new_server)->SetRegisterState(IClient::RegisterState::kRegistering);
	(*new_server)->Push("SERVER " + server_config_->GetName() + " 1 :" + server_config_->GetDescription());
}
