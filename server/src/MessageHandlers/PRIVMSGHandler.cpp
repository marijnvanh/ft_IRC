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

PRIVMSGHandler::PRIVMSGHandler(IServerConfig *server_config, IClientDatabase *client_database, IChannelDatabase *channel_database) :
	CommandHandler(server_config, client_database, "PRIVMSG"),
    channel_database_(channel_database)
{}

PRIVMSGHandler::~PRIVMSGHandler()
{}

auto PRIVMSGHandler::SafeHandle(IMessage &message) -> void
{
    auto client = *(client_database_->GetClient(message.GetClientUUID()));

    if (!ValidateParams(client, message))
        return ;

    /* Get the remote user and set client to remote user */
    if (client->GetType() == IClient::Type::kLocalServer)
    {
        auto remote_client_nickname = message.GetNickname();
    
        if (remote_client_nickname == std::nullopt)
        {
            client->Push(FormatERRORMessage(client->GetPrefix(), "PRIVMSG No Nickname given"));
            return ;
        }
        auto remote_client = client_database_->GetClient(*remote_client_nickname);
        if (remote_client == std::nullopt)
        {
            client->Push(FormatERRORMessage(client->GetPrefix(), "No such nick: " + *remote_client_nickname));
            return ;
        }
        client = *remote_client;
    }
    SendMessages(client, message);
}

auto PRIVMSGHandler::FormatForwardList(std::vector<std::string> &forward_list) -> std::string
{
    std::string ret;
    for (std::string receiver : forward_list)
    {
        ret = ret + receiver + ",";
    }
    if (ret.size() > 0)
        ret.pop_back();
    return ret;
}

auto PRIVMSGHandler::ValidateParams(IClient *client, IMessage &message) -> bool
{
    auto params = message.GetParams();
    if (params.size() < 2)
    {
        if (client->GetType() == IClient::Type::kLocalUser)
            client->Push(GetErrorMessage(server_config_->GetName(), client->GetPrefix(), ERR_NEEDMOREPARAMS, "PRIVMSG"));
        else if (client->GetType() == IClient::Type::kLocalServer)
            client->Push(FormatERRORMessage(client->GetPrefix(), "PRIVMSG needs more params"));
        return false;
    }
    if (params[MESSAGE_CONTENT] == "") 
    {
        if (client->GetType() == IClient::Type::kLocalUser)
            client->Push(GetErrorMessage(server_config_->GetName(), client->GetPrefix(), ERR_NOTEXTTOSEND, "PRIVMSG"));
        else if (client->GetType() == IClient::Type::kLocalServer)
            client->Push(FormatERRORMessage(client->GetPrefix(), "PRIVMSG no text to send"));
        return false;
    }
    return true;
}

auto PRIVMSGHandler::SendMessages(IClient *sender, IMessage &message) -> void
{
    std::vector<std::string> forward_list;
    auto params = message.GetParams();
    auto receivers = split(params[LIST_OF_RECEIVERS], ",");

    for (std::string receiver : receivers)
    {
        if (receiver[0] == '#')
        {
            PRIVMSGToChannel(sender, receiver, params[MESSAGE_CONTENT]);
            forward_list.push_back(receiver);
        }
        else
        {
            if (PRIVMSGToUser(sender, receiver, params[MESSAGE_CONTENT]) == false)
                forward_list.push_back(receiver);
        }
    }
    if (forward_list.size() > 0)
    {
        auto receivers = FormatForwardList(forward_list);
        auto privmsg = ":" + sender->GetNickname() + " PRIVMSG " + receivers + " :" + params[MESSAGE_CONTENT];
        client_database_->BroadcastToLocalServers(privmsg, message.GetClientUUID());
    }
}

auto PRIVMSGHandler::PRIVMSGToUser(IClient *sender, const std::string &nickname, std::string message_content) -> bool
{
    auto user = client_database_->GetUser(nickname);
    if (user == std::nullopt)
    {
        sender->Push(GetErrorMessage(server_config_->GetName(), sender->GetPrefix(), ERR_NOSUCHNICK, nickname));
        return true;
    }
    else if ((*user)->GetType() == IClient::Type::kLocalUser)
    {
        auto response = ":" + sender->GetNickname() + " PRIVMSG " + nickname +  " :" + message_content;
        (*user)->Push(response);
        return true;
    }
    else /* user is a remote user so we don't do anything */
        return false;
}

auto PRIVMSGHandler::PRIVMSGToChannel(IClient *sender, const std::string &channel_name, std::string message_content) -> void
{
    auto channel = channel_database_->GetChannel(channel_name);

    if (channel == std::nullopt)
    {
        sender->Push(GetErrorMessage(server_config_->GetName(), sender->GetPrefix(), ERR_NOSUCHCHANNEL, channel_name));
        return ;
    }
    else
    {
        if ((*channel)->HasUser(sender->GetUUID()))
        {
            auto privmsg = ":" + sender->GetPrefix() + " PRIVMSG " + channel_name +  " :" + message_content;
            (*channel)->PushToLocal(privmsg, std::optional<IRC::UUID>(sender->GetUUID()));
        }
        else
        {
            sender->Push(GetErrorMessage(server_config_->GetName(), sender->GetPrefix(), ERR_NOTONCHANNEL, channel_name));
        }
    }
}
