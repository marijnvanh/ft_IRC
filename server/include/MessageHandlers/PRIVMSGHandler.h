#ifndef PRIVMSG_HANDLER_HPP
#define PRIVMSG_HANDLER_HPP

#include "MessageHandlers/ICommandHandler.h"
#include "IClientDatabase.h"
#include "IChannelDatabase.h"
#include "Logger.h"

class PRIVMSGHandler : public ICommandHandler
{
    public:
    PRIVMSGHandler(IClientDatabase *client_database, IChannelDatabase *channel_database);
    ~PRIVMSGHandler();

    auto Handle(IMessage &message) -> void override;

    private:
    IClientDatabase *client_database_;
    IChannelDatabase *channel_database_;
    Logger logger;

    auto HandlePrivmsg(IClient *sender, IMessage &message) -> void;
    auto PRIVMSGToUser(IClient *sender, std::string &receiver, std::string message_content) -> void;
    auto PRIVMSGToChannel(IClient *sender, std::string &receiver, std::string message_content) -> void;
};

#endif
