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

    /**
     * @brief 
     * 
     * @param sender 
     * @param message 
     * @return std::vector<std::string> List with names that need to be forwarded
     */
    auto SendMessages(IClient *sender, IMessage &message) -> void;
    /**
     * @brief 
     * 
     * @param client 
     * @param message 
     * @return true params are valid
     * @return false params are invalid
     */
    auto ValidateParams(IClient *client, IMessage &message) -> bool;
    auto FormatForwardList(std::vector<std::string> &forward_list) -> std::string;
    auto PRIVMSGToUser(IClient *sender, const std::string &nickname, std::string message_content) -> bool;
    auto PRIVMSGToChannel(IClient *sender, const std::string &channel_name, std::string message_content) -> void;
};

#endif
