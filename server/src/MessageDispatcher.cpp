#include "MessageDispatcher.h"
#include "MessageHandlers/PingPongHandler.h"
#include "MessageHandlers/PASSHandler.h"

MessageDispatcher::MessageDispatcher(std::shared_ptr<ServerData> server_data) 
    : server_data_(server_data)
{
    handlers_.insert(std::make_pair("ping", [](auto server_data, auto message) {
            PingPongHandler(server_data->client_database_, message);
        }));
    handlers_.insert(std::make_pair("PASS", [](auto server_data, auto message) {
            std::shared_ptr<IRC::Mutex<IClient>> client;
            try {
                client = server_data->client_database_->GetClient(message.GetUUID());
            } catch (IClientDatabase::ClientNotFound &ex) {
                return ;
            }
            PASSHandler(client, message);
        }));
}

MessageDispatcher::~MessageDispatcher()
{}

auto MessageDispatcher::Dispatch(Message message) -> void
{
    auto handler = handlers_.find(message.GetCommand());

    if (handler != handlers_.end())
        handler->second(server_data_, message);
    else
        std::cerr << "Received invalid message" << std::endl;
    //TODO Handle invalid message
}
