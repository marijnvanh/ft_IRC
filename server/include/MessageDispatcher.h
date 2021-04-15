#ifndef MESSAGE_DISPATCHER_HPP
#define MESSAGE_DISPATCHER_HPP

#include <unordered_map>
#include <memory>

#include "MessageHandlers/ICommandHandler.h"
#include "IMessageDispatcher.h"
#include "ServerData.h"
#include "Logger.h"
#include "IRCServer.h"

class IRCServer;

class MessageDispatcher : public IMessageDispatcher
{
    public:
    MessageDispatcher(ServerData *server_data, IRCServer *irc_server);
    ~MessageDispatcher();

    /**
     * @brief Dispatch message to its appropriate message handler
     * Will return true on success and false if it cannot match a message handler
     * 
     * @param message 
     */
    auto Dispatch(Message message) -> bool override;

    private:
    std::unordered_map<std::string, std::unique_ptr<ICommandHandler>> command_handlers_;
    Logger logger;
};

#endif