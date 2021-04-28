#include "Utilities.h"
#include "Numerics.h"

#include <unordered_map>

std::unordered_map<int, std::string> error_map = {
        {ERR_NOTREGISTERED, "You have not registered"},
        {ERR_NONICKNAMEGIVEN, "No nickname given"},
        {ERR_NOSUCHNICK, "No such nick/channel"},
        {ERR_NEEDMOREPARAMS, "Not enough parameters"},
        {ERR_NOTEXTTOSEND, "No text to send"},
        {ERR_UNKNOWNCOMMAND, "Unknown command"},
        {ERR_NOSUCHCHANNEL, "No such channel"},
		{ERR_BADCHANNELKEY, "Cannot join channel (+k)"},
		{ERR_UMODEUNKNOWNFLAG, "Unknown MODE flag by us"},
		{ERR_USERSDONTMATCH, "Cannot change mode for other users"},
		{ERR_CHANOPRIVSNEEDED, "You're not channel operator"},
		{ERR_NOTONCHANNEL, "You're not on that channel"},
        {ERR_ALREADYREGISTERED, "You may not reregister"},
        {ERR_ERRONEUSNICKNAME, "Erroneus nickname"},
        {ERR_NICKNAMEINUSE, "Nickname is already in use"},
        {ERR_NICKCOLLISION, "Nickname collision KILL"},
		{ERR_CANTKILLSERVER, "You cant kill a server!"},
		{ERR_NOPRIVILEGES, "Permission Denied- You're not an IRC operator"},
        {ERR_NOSUCHSERVER, "No such server"},
		{ERR_NOOPERHOST, "No O-lines for your host"},
		{ERR_PASSWDMISMATCH, "Password incorrect"}
};

auto GetErrorMessage(const std::string &prefix, int error, std::string param) -> std::string
{
    auto formatted_prefix = ":" + prefix + " ";

    auto error_pair = error_map.find(error);

    if (error_pair == error_map.end())
        return formatted_prefix + "ERROR :Unknown Error";

    if (param != "")
        return formatted_prefix + std::to_string(error) + " " + param + " :" + error_pair->second;
    else
        return formatted_prefix + std::to_string(error) + " :" + error_pair->second;
}
