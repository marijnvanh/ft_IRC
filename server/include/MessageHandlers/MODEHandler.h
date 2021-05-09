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
		std::vector<std::string> param) -> void;
    
	  auto HandleMODEChannel(IUser *user,
		std::vector<std::string> param) -> void;

 	private:

    IChannelDatabase *channel_database_;

    auto FormatMode(std::bitset<64> mode) -> std::string;
    auto HandleChannelTopicSet(IUser *user, IChannel *channel,
        std::vector<std::string> params, bool set) -> void;
    auto HandleChannelKeySet(IUser *user, IChannel *channel,
        std::vector<std::string> params, bool set) -> void;
    auto HandleChannelOperatorSet(IUser *user, IChannel *channel,
	    IClientDatabase *client_database, std::vector<std::string> params, bool set) -> void;
};

#endif
