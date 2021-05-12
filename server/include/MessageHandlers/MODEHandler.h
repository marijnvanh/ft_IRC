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

    auto HandleMODEUser(IUser *user,
		std::vector<std::string> &params) -> void;
    
	  auto HandleMODEChannel(IUser *user,
		std::vector<std::string> &params) -> void;

 	private:

    IChannelDatabase *channel_database_;

    auto FormatMode(std::bitset<64> mode) -> std::string;
    auto HandleChannelTopicSet(IChannel *channel,
        std::optional<std::string> param, bool set) -> void;
    auto HandleChannelKeySet(IChannel *channel,
		std::optional<std::string> param, bool set) -> void;
    auto HandleChannelOperatorSet(IUser *user, IChannel *channel,
		std::optional<std::string> param, bool set) -> void;
};

#endif
