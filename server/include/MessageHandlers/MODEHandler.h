#ifndef MODE_HANDLER_HPP
#define MODE_HANDLER_HPP

#include <bitset>

#include "Logger.h"
#include "IServerConfig.h"
#include "IClientDatabase.h"
#include "IChannelDatabase.h"
#include "MessageHandlers/ICommandHandler.h"

class MODEHandler : public ICommandHandler
{

public:

    MODEHandler(IClientDatabase *client_database, IChannelDatabase *channel_database);
    ~MODEHandler();

    auto Handle(IMessage &message) -> void override;

    auto HandleMODEUser(IUser *user,
		std::vector<std::string> param) -> void;
    
	auto HandleMODEChannel(IUser *user,
		std::vector<std::string> param) -> void;

 private:

    Logger logger;
    IClientDatabase *client_database_;
    IChannelDatabase *channel_database_;

	auto FormatMode(std::bitset<64> mode) -> std::string;

};

#endif
