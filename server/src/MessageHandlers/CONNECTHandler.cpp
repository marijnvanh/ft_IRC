#include "Numerics.h"
#include "Utilities.h"
#include "MessageHandlers/CONNECTHandler.h"

#define SERVER_NAME_PARAM 0

CONNECTHandler::CONNECTHandler(IServerConfig *server_config, IClientDatabase *client_database, IRCServer *irc_server) :
    CommandHandler(server_config, client_database, "CONNECT", 1),
    irc_server_(irc_server)
{}

CONNECTHandler::~CONNECTHandler()
{}

auto CONNECTHandler::SafeHandle(IMessage &message) -> void
{
	auto params = message.GetParams();
    auto client = *(client_database_->GetClient(message.GetClientUUID()));

	if (client->GetType() == IClient::Type::kLocalServer)
	{
		client->Push("ERROR :CONNECT from server not supported");
        return ;
    }
	auto user = dynamic_cast<IUser*>(client);
	if (!user->HasMode(UserMode::UM_OPERATOR))
	{
		client->Push(GetErrorMessage(server_config_->GetName(), ERR_NOPRIVILEGES, "CONNECT"));
		return ;
	}

	auto authorized_server_data =
		server_config_->GetAuthorizedServer(params[SERVER_NAME_PARAM]);
    if (!authorized_server_data)
	{
		client->Push(GetErrorMessage(server_config_->GetName(), ERR_NOSUCHSERVER, params[SERVER_NAME_PARAM]));
        return ;
    }

    auto new_server = irc_server_->CreateNewConnection(
		(*authorized_server_data)->GetIP(),
		(*authorized_server_data)->GetPort());
	if (!new_server)
	{
		logger_.Log(LogLevel::WARNING, "Failed to connect to server: %s", params[SERVER_NAME_PARAM].c_str());
        return ;
    }
	(*new_server)->SetRegisterState(IClient::RegisterState::kRegistering);

	(*new_server)->Push(":" + server_config_->GetName() + " PASS " +
		(*authorized_server_data)->GetSendPass() + " 0210-IRC+ codIrcd|");
	(*new_server)->Push(":" + server_config_->GetName() + " SERVER " +
		server_config_->GetName() + " 1 :" + server_config_->GetDescription());
}
