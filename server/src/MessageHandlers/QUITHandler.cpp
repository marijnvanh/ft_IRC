#include "MessageHandlers/QUITHandler.h"
#include "Numerics.h"
#include "IServer.h"
#include "ILocalUser.h"
#include "IRemoteUser.h"

#define DEFAULT_QUIT_MESSAGE "Leaving"

//TODO Still a lot of todo's here

static auto GetQuitMessage(IMessage &message) -> std::string
{
    auto params = message.GetParams();
    if (params.size() == 0)
        return DEFAULT_QUIT_MESSAGE;
    else
        return params.front();
}

static auto DisconnectLocalUser(std::shared_ptr<IClientDatabase> client_database, IMessage &message) -> void
{
    auto client = message.GetClient();
    auto quit_message = GetQuitMessage(message);
    // auto local_user = std::dynamic_pointer_cast<ILocalUser>(client);
    //TODO send quit message to all local Users in all the channels from local_user.GetChannels()
    //TODO remove from IServer it is connected to
    client_database->RemoveUser(client->GetNickname());
}

static auto DisconnectRemoteUser(std::shared_ptr<IClientDatabase> client_database, IMessage &message) -> void
{
    auto nickname = message.GetNickname();
    if (nickname == std::nullopt)
        return ; //TODO error
    //TODO validate nickname    

    auto quit_message = GetQuitMessage(message);
    
    auto remote_client = client_database->GetClient(*nickname);
    if (remote_client && (*remote_client)->GetType() != IClient::Type::kRemoteUser)
    {
        //TODO send quit message to all local Users in all the channels from user.GetChannels()
        //TODO remove from IServer it is connected to
        client_database->RemoveUser(*nickname);
    }
    else
        ; //TODO error no such client

}

auto QUITHandler(std::shared_ptr<IClientDatabase> client_database, IMessage &message) -> void
{
    auto client = message.GetClient();

    if (client->GetState() == IClient::State::kUnRegistered)
    {
        client_database->RemoveClient(client->GetUUID());
    }
    else if (client->GetType() == IClient::Type::kLocalUser)
    {
        DisconnectLocalUser(client_database, message);
    }
    else if (client->GetType() == IClient::Type::kServer)
    {
        DisconnectRemoteUser(client_database, message);
    }
}
