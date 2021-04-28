#ifndef MODE_HANDLER_HPP
#define MODE_HANDLER_HPP

#include <bitset>

#include "IServerConfig.h"
#include "CommandHandler.h"
#include "IChannelDatabase.h"

class MODEHandler : public CommandHandler
{
	public:

    MODEHandler(IClientDatabase *client_database, IChannelDatabase *channel_database);
    ~MODEHandler();

    auto SafeHandle(IMessage &message) -> void override;

    auto HandleMODEUser(IUser *user,
		std::vector<std::string> param) -> void;
    
	auto HandleMODEChannel(IUser *user,
		std::vector<std::string> param) -> void;

 	private:

    IChannelDatabase *channel_database_;

	auto FormatMode(std::bitset<64> mode) -> std::string;

};

#endif
