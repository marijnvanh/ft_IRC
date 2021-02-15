#include "MessageDispatcher.h"
#include "MessageHandlers/PingPongHandler.h"

MessageDispatcher::MessageDispatcher(std::shared_ptr<ServerData> server_data) 
    : server_data_(server_data)
{
    handlers_.insert(std::make_pair("ping", [](auto server_data, auto message) {
            PingPongHandler(server_data->client_database_, message);
        }));
}

MessageDispatcher::~MessageDispatcher()
{}

auto MessageDispatcher::Dispatch(std::string message) -> void
{
    auto handler = handlers_.find(message);

    if (handler != handlers_.end())
        handler->second(server_data_, message);
    else
        std::cerr << "Received invalid message" << std::endl;
    //TODO Handle invalid message
}
