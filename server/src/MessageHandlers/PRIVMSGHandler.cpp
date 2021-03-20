#include "MessageHandlers/PRIVMSGHandler.h"
#include "Numerics.h"
#include "Utilities.h"

#define LIST_OF_RECEIVERS 0
#define MESSAGE_CONTENT 1

/*
      Command: PRIVMSG
   Parameters: <receiver>{,<receiver>} <text to be sent>

   PRIVMSG is used to send private messages between users.  <receiver>
   is the nickname of the receiver of the message.  <receiver> can also
   be a list of names of channels separated with commas.

   Examples:

:Angel PRIVMSG Wiz :Hello are you receiving this message ?
                                ; Message from Angel to Wiz.

PRIVMSG Angel :yes I'm receiving it ;
                                Message to Angel.

:Angel PRIVMSG #channel1 :Hello anyone there ?
                                ; Message from Angel to #channel1.
*/

PRIVMSGHandler::PRIVMSGHandler(IClientDatabase *client_database, IChannelDatabase *channel_database) :
    client_database_(client_database),
    channel_database_(channel_database),
    logger("PRIVMSGHandler")
{}

PRIVMSGHandler::~PRIVMSGHandler()
{}

auto PRIVMSGHandler::Handle(IMessage &message) -> void
{
    auto client = *(client_database_->GetClient(message.GetClientUUID()));

    if (client->GetState() == IClient::State::kUnRegistered)
    {
        client->Push(GetErrorMessage(ERR_NOTREGISTERED));
        return ;
    }
    
    if (client->GetType() == IClient::Type::kServer)
    {
        auto remote_client_nickname = message.GetNickname();
    
        if (remote_client_nickname == std::nullopt)
        {
            client->Push(GetErrorMessage(ERR_NONICKNAMEGIVEN)); //TODO
            return ;
        }
        //TODO validate nickname
        auto remote_client = client_database_->GetClient(*remote_client_nickname);
        if (remote_client == std::nullopt)
        {
            client->Push(GetErrorMessage(ERR_NOSUCHNICK , *remote_client_nickname));
            return ;
        }
        client = *remote_client;
    }
    StartParamChecks(client, message);
}

auto PRIVMSGHandler::StartParamChecks(IClient *sender, IMessage &message) -> void
{
    auto params = message.GetParams();
    if (params.size() < 2)
    {
        sender->Push(GetErrorMessage(ERR_NEEDMOREPARAMS, "PRIVMSG"));
        return ;
    }
    if (params[MESSAGE_CONTENT] == "") 
    {
        sender->Push(GetErrorMessage(ERR_NOTEXTTOSEND));
        return ;
    }

    auto receivers = split(params[LIST_OF_RECEIVERS], ",");

    for (std::string receiver : receivers)
    {
        //TODO validate nickname / channel (receiver)
        if (receiver[0] == '#')
            PRIVMSGToChannel(sender, receiver, params[MESSAGE_CONTENT]);
        else
            PRIVMSGToUser(sender, receiver, params[MESSAGE_CONTENT]);
    }
}

auto PRIVMSGHandler::PRIVMSGToUser(IClient *sender, std::string &receiver, std::string message_content) -> void
{
    auto client = client_database_->GetUser(receiver);
    if (client == std::nullopt)
        sender->Push(GetErrorMessage(ERR_NOSUCHNICK, receiver));
    else
    {
        auto response = ":" + sender->GetNickname() + " PRIVMSG " + receiver +  " :" + message_content;
        (*client)->Push(response);
    }
}

//TODO ERR_CANNOTSENDTOCHAN
//TODO RPL_AWAY
auto PRIVMSGHandler::PRIVMSGToChannel(IClient *sender, std::string &receiver, std::string message_content) -> void
{
    auto channel = channel_database_->GetChannel(receiver);

    if (channel == std::nullopt)
    {
        sender->Push(GetErrorMessage(ERR_NOSUCHCHANNEL, receiver));
        return ;
    }
    else
    {
        auto response = ":" + sender->GetNickname() + " PRIVMSG " + receiver +  " :" + message_content;
        (*channel)->PushToLocal(response); //TODO except current user?
        //TODO send msg to all servers connected to this server except from the user it came from
    }
}
