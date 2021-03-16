#include "MessageHandlers/PRIVMSGHandler.h"
#include "Numerics.h"
#include "Utilities.h"

#include <iostream>

#define LIST_OF_RECEIVERS 0
#define MESSAGE_CONTENT 1
/*
      Command: PRIVMSG
   Parameters: <receiver>{,<receiver>} <text to be sent>

   PRIVMSG is used to send private messages between users.  <receiver>
   is the nickname of the receiver of the message.  <receiver> can also
   be a list of names or channels separated with commas.

   Numeric Replies:

           ERR_NORECIPIENT                 ERR_NOTEXTTOSEND
           ERR_CANNOTSENDTOCHAN            ERR_NOTOPLEVEL
           ERR_NOSUCHNICK
           RPL_AWAY

   Examples:

:Angel PRIVMSG Wiz :Hello are you receiving this message ?
                                ; Message from Angel to Wiz.

PRIVMSG Angel :yes I'm receiving it !receiving it !'u>(768u+1n) .br ;
                                Message to Angel.
*/

//TODO fix it
static auto SendMessages(IClient *sender, IClientDatabase *client_database, std::vector<std::string> receivers, std::string message_content) -> void
{
    for (std::string receiver : receivers)
    {
        //TODO validate nickname / channel (receiver)
        if (true) // if nickname
        {
            auto client = client_database->GetUser(receiver);
            if (client == std::nullopt)
                sender->Push(std::to_string(ERR_NOSUCHNICK) + receiver + " :No such nick/channel"); //TODO
            else
            {
                auto response = ":" + sender->GetNickname() + " PRIVMSG " + receiver +  ":" + message_content;
                (*client)->Push(response);
            }
        }
        else // handle channels
        {}

    }
}

static auto HandlePrivmsg(IClient *sender, IClientDatabase *client_database, IMessage &message) -> void
{
    auto params = message.GetParams();
    if (params.size() < 2)
    {
        sender->Push(std::to_string(ERR_NEEDMOREPARAMS) + "PRIVMSG :Not enough parameters"); //TODO
        return ;
    }
    if (params[MESSAGE_CONTENT] == "") 
    {
        sender->Push(std::to_string(ERR_NOTEXTTOSEND) + ":No text to send"); //TODO
        return ;
    }

    auto receivers = split(params[LIST_OF_RECEIVERS], ",");

    SendMessages(sender, client_database, receivers, params[MESSAGE_CONTENT]);
}

auto PRIVMSGHandler(IClientDatabase *client_database, IMessage &message) -> void
{
    auto client = *(client_database->GetClient(message.GetClientUUID()));

    if (client->GetState() == IClient::State::kUnRegistered)
    {
        client->Push(std::to_string(ERR_NOTREGISTERED) + ":You have not registered"); //TODO
        return ;
    }
    
    if (client->GetType() == IClient::Type::kServer)
    {
        auto remote_client_nickname = message.GetNickname();
    
        if (remote_client_nickname == std::nullopt)
        {
            client->Push(std::to_string(ERR_NONICKNAMEGIVEN) + ":No nickname given"); //TODO
            return ;
        }
        //TODO validate nickname
        auto remote_client = client_database->GetClient(*remote_client_nickname);
        if (remote_client == std::nullopt)
        {
            client->Push(std::to_string(ERR_NOSUCHNICK) + *remote_client_nickname + " :Can't find nickname"); //TODO
            return ;
        }
        client = *remote_client;
    }

    HandlePrivmsg(client, client_database, message);
}
