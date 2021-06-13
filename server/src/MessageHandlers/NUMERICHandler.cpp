#include "Numerics.h"
#include "Utilities.h"
#include "MessageHandlers/NUMERICHandler.h"
#include "RawMessage.h"

#define PARAM_TARGET 0

NUMERICHandler::NUMERICHandler(IServerConfig* server_config, IClientDatabase *client_database) :
	CommandHandler(server_config, client_database, "NUMERIC", 0, true)
{}

NUMERICHandler::~NUMERICHandler()
{}

static auto FormNumericMessage(IMessage &message) -> std::string
{
    auto prefix = *(message.GetServername());
    auto message_params = message.GetParams();

    auto numeric_message = ":" + prefix + " " + message.GetCommand() + " " + message_params[PARAM_TARGET];

    auto param_count = message_params.size() - 1; /* -1 for PARAM_TARGET */
    size_t param_index = 1; /* index 1 so we skip PARAM_TARGET */
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

auto NUMERICHandler::SafeHandle(IMessage &message) -> void
{
	/* If numeric does not come from a server we ignore it */
    auto client = *(client_database_->GetClient(message.GetClientUUID()));
	if (client->GetType() != IClient::Type::kLocalServer) {
        logger_.Log(LogLevel::DEBUG, "Received numeric from non server");
		return;
    }

    auto origin = message.GetServername();
    if (!origin)
	{
		client->Push(FormatERRORMessage(client->GetPrefix(), "No prefix provided to numeric message"));
		return;
	}

    auto params = message.GetParams();
    if (params.size() < 1)
	{
		client->Push(FormatERRORMessage(client->GetPrefix(), "No target for numeric message"));
		return;
	}

    /* Quick and ugly hack to parse the target of numeric, should fix this */
    auto cs = IRC::Parser::CharStream::FromString(params[PARAM_TARGET]);
    std::string nickname;
    try {
        auto prefix = IRC::ParsePrefix(cs);
        nickname = prefix.name;
    } 
    catch (...) {
        logger_.Log(LogLevel::DEBUG, "Failed to parse 'prefix': %s", params[PARAM_TARGET].c_str());
    }
    if (nickname == "")
        nickname = params[PARAM_TARGET];

    auto target = client_database_->GetClient(nickname);
    if (!target)
	{
		client->Push(FormatERRORMessage(client->GetPrefix(), "Unknown target: " + params[PARAM_TARGET]));
		return;
	}

    (*target)->Push(FormNumericMessage(message));
    logger_.Log(LogLevel::DEBUG, "Forwarded numeric reply %s", message.GetCommand().c_str());
}
