#ifndef MODE_HANDLER_HPP
#define MODE_HANDLER_HPP

#include <bitset>

#include "CommandHandler.h"
#include "IChannelDatabase.h"

class MODEHandler : public CommandHandler
{
	public:

    MODEHandler(IServerConfig *server_config, IClientDatabase *client_database, IChannelDatabase *channel_database);
    ~MODEHandler();

    auto SafeHandle(IMessage &message) -> void override;

    auto HandleMODEUser(IClient *client, IMessage &message) -> void;
	auto HandleMODEUser_PreCheck(IClient *origin, IMessage &message,
		IUser **target) -> bool;
    
	auto HandleMODEChannel(IClient *client,
		std::vector<std::string> &params) -> void;

 	private:

    IChannelDatabase *channel_database_;

    auto HandleChannelKeySet(IChannel *channel,
		std::optional<std::string> param, bool set) -> void;
    auto HandleChannelOperatorSet(IClient *client, IChannel *channel,
		std::optional<std::string> param, bool set) -> bool;

	auto FormatMode(std::bitset<64> mode) -> std::string;

	auto GetOriginalSender(IClient **client, IMessage &message) -> bool;
};

#endif
