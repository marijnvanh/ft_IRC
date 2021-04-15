#include "Numerics.h"
#include "Utilities.h"
#include "MessageHandlers/NUMERICHandler.h"

#define PARAM_ERROR_MESSAGE 0

NUMERICHandler::NUMERICHandler(IClientDatabase *client_database)
	: client_database_(client_database),
    logger("NUMERICHandler")
{}

NUMERICHandler::~NUMERICHandler()
{}

static auto FormNumericMessage(IMessage &message) -> std::string
{
    auto nickname = *(message.GetNickname());
    auto numeric_message = ":" + nickname + " " + message.GetCommand();

    auto message_params = message.GetParams();
    auto param_count = message_params.size();
    size_t param_index = 0;
    while (param_count && param_count - 1 > 0)
    {
        numeric_message = numeric_message + " " + message_params[param_index];
        param_index++;
        param_count--;
    }
    if (param_count == 1)
        numeric_message = numeric_message + " :" + message_params[param_index];
    return numeric_message;
}

auto NUMERICHandler::Handle(IMessage &message) -> void
{
	/* If numeric does not come from a server we ignore it */
    auto client = *(client_database_->GetClient(message.GetClientUUID()));
	if (client->GetType() != IClient::Type::kLocalServer)
		return;

    auto nickname = message.GetNickname();
    if (!nickname)
	{
		client->Push("ERROR :No nickname provided to numeric message");
		return;
	}
    
    auto receiver = client_database_->GetClient(*nickname);
    if (!receiver)
	{
		client->Push("ERROR :Unknown client with nickname: " + *nickname);
		return;
	}

    (*receiver)->Push(FormNumericMessage(message));
    logger.Log(LogLevel::DEBUG, "Forwarded numeric reply %s", message.GetCommand().c_str());
}
