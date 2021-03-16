#include "MessageHandlers/PRIVMSGHandler.h"
#include "Numerics.h"

#include <iostream>

/*
      Command: PRIVMSG
   Parameters: <receiver>{,<receiver>} <text to be sent>

   PRIVMSG is used to send private messages between users.  <receiver>
   is the nickname of the receiver of the message.  <receiver> can also
   be a list of names or channels separated with commas.

   The <receiver> parameter may also me a host mask  (#mask)  or  server
   mask  ($mask).   In  both cases the server will only send the PRIVMSG
   to those who have a server or host matching the mask.  The mask  must
   have at  least  1  (one)  "."  in it and no wildcards following the
   last ".".  This requirement exists to prevent people sending messages
   to  "#*"  or "$*",  which  would  broadcast  to  all  users; from
   experience, this is abused more than used responsibly and properly.
   Wildcards are  the  '*' and  '?'   characters.   This  extension  to
   the PRIVMSG command is only available to Operators.

   Numeric Replies:

           ERR_NORECIPIENT                 ERR_NOTEXTTOSEND
           ERR_CANNOTSENDTOCHAN            ERR_NOTOPLEVEL
           ERR_WILDTOPLEVEL                ERR_TOOMANYTARGETS
           ERR_NOSUCHNICK
           RPL_AWAY

   Examples:

:Angel PRIVMSG Wiz :Hello are you receiving this message ?
                                ; Message from Angel to Wiz.

PRIVMSG Angel :yes I'm receiving it !receiving it !'u>(768u+1n) .br ;
                                Message to Angel.

PRIVMSG jto@tolsun.oulu.fi :Hello !
                                ; Message to a client on server
                                tolsun.oulu.fi with username of "jto".

PRIVMSG $*.fi :Server tolsun.oulu.fi rebooting.
                                ; Message to everyone on a server which
                                has a name matching *.fi.

PRIVMSG #*.edu :NSFNet is undergoing work, expect interruptions
                                ; Message to all users who come from a
                                host which has a name matching *.edu.
*/

//TODO improve

std::vector<std::string> split(const std::string& str, const std::string& delim)
{
    std::vector<std::string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == std::string::npos) pos = str.length();
        std::string token = str.substr(prev, pos-prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    }
    while (pos < str.length() && prev < str.length());
    return tokens;
}

static auto SendMessages(IClient *sender, IClientDatabase *client_database, std::vector<std::string> receivers, std::string message) -> void
{
    message = ":" + sender->GetNickname() + " PRIVMSG " + message; //TODO fix this

    for (std::string nickname : receivers)
    {
        auto receiver = client_database->GetUser(nickname);

        if (receiver == std::nullopt)
        {
            ; //TODO send error back
            std::cout << "Can't send to: [" << nickname << "]" << std::endl;

        }
        else
        {
            std::cout << "Sending: " << message << " to " << nickname << std::endl;
            (*receiver)->Push(message);
        }
    }
}

static auto HandlePrivmsg(IClient *sender, IClientDatabase *client_database, IMessage &message) -> void
{
    auto params = message.GetParams();
    if (params.size() < 2)
    {
        sender->Push(std::to_string(ERR_NEEDMOREPARAMS) + "Need more params"); //TODO
        return ;
    }

    // Parse first param into list of nicknames / channels

    // Handle channels

    // Handle users
    auto receivers = split(params[0], ",");
    SendMessages(sender, client_database, receivers, params[1]);

}

auto PRIVMSGHandler(IClientDatabase *client_database, IMessage &message) -> void
{
    auto client = *(client_database->GetClient(message.GetClientUUID()));

    if (client->GetState() == IClient::State::kUnRegistered)
    {
        client->Push(std::to_string(ERR_NOTREGISTERED) + "Not Registered"); //TODO
        return ;
    }
    
    if (client->GetType() == IClient::Type::kServer)
    {
        auto remote_client_nickname = message.GetNickname();
    
        if (remote_client_nickname == std::nullopt)
        {
            client->Push(std::to_string(ERR_NONICKNAMEGIVEN) + "No nickname given"); //TODO
            return ;
        }
        //TODO validate nickname
        auto remote_client = client_database->GetClient(*remote_client_nickname);
        if (remote_client == std::nullopt)
        {
            client->Push(std::to_string(ERR_NOSUCHNICK) + "Can't find nickname"); //TODO
            return ;
        }
        client = *remote_client;
    }

    HandlePrivmsg(client, client_database, message);
    auto params = message.GetParams();
    std::cout << "params: " << std::endl;
    for (std::string str : params)
        std::cout << str << std::endl;
}
