#include "MessageHandlers/QUITHandler.h"
#include "Numerics.h"
#include "Utilities.h"
#include "IServer.h"
#include "ILocalUser.h"
#include "IRemoteUser.h"

#define DEFAULT_QUIT_MESSAGE "Leaving"


QUITHandler::QUITHandler(IClientDatabase *client_database) :
    client_database_(client_database),
    logger("QUITHandler")
{}

QUITHandler::~QUITHandler()
{}

//TODO Still a lot of todo's here
auto QUITHandler::Handle(IMessage &message) -> void
{
    auto client = *(client_database_->GetClient(message.GetClientUUID()));

    if (client->GetState() == IClient::State::kUnRegistered)
    {
        client_database_->DisconnectClient(client->GetUUID());
    }
    else if (client->GetType() == IClient::Type::kLocalUser)
    {
        DisconnectLocalUser(message);
    }
    else if (client->GetType() == IClient::Type::kServer)
    {
        DisconnectRemoteUser(client, message);
    }
}

auto QUITHandler::GetQuitMessage(IMessage &message) -> std::string
{
    auto params = message.GetParams();
    if (params.size() == 0)
        return DEFAULT_QUIT_MESSAGE;
    else
        return params.front();
}

auto QUITHandler::DisconnectLocalUser(IMessage &message) -> void
{
    auto quit_message = GetQuitMessage(message);
    // auto local_user = std::dynamic_pointer_cast<ILocalUser>(client);
    //TODO send quit message to all local Users in all the channels from local_user.GetChannels()
    //TODO remove from IServer it is connected to
    client_database_->DisconnectClient(message.GetClientUUID());
}

auto QUITHandler::DisconnectRemoteUser(IClient* remote_user, IMessage &message) -> void
{
    auto nickname = message.GetNickname();
    if (nickname == std::nullopt)
    {
        remote_user->Push(GetErrorMessage(ERR_NONICKNAMEGIVEN));
        return ;
    }
    //TODO validate nickname    

    auto quit_message = GetQuitMessage(message);
    
    auto remote_client = client_database_->GetClient(*nickname);
    if (remote_client && (*remote_client)->GetType() != IClient::Type::kRemoteUser)
    {
        //TODO send quit message to all local Users in all the channels from user.GetChannels()
        //TODO remove from IServer it is connected to
        client_database_->DisconnectClient((*remote_client)->GetUUID());
    }
    else
    {
        remote_user->Push(GetErrorMessage(ERR_NOSUCHNICK));
        return ;
    }
}
