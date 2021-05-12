#ifndef TOPIC_HANDLER_HPP
#define TOPIC_HANDLER_HPP

#include "CommandHandler.h"
#include "IChannelDatabase.h"

class TOPICHandler : public CommandHandler
{
    public:
    TOPICHandler(IServerConfig *server_config,
		IClientDatabase *client_database,
		IChannelDatabase *channel_database);
    ~TOPICHandler();

    auto SafeHandle(IMessage &message) -> void override;

	auto GetCorrectSender(IClient **client, IMessage &message) -> bool;

	private:
	IChannelDatabase *channel_database_;
};

#endif
