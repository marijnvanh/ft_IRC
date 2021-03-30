#include "Utilities.h"
#include "Numerics.h"

#include <unordered_map>

std::unordered_map<int, std::string> error_map = {
        {ERR_NOSUCHNICK, ":No such nick/channel"},
        {ERR_NOSUCHSERVER, ":No such server"},
        {ERR_NOSUCHCHANNEL, ":No such channel"},
        {ERR_NOTEXTTOSEND, ":No test to send"},
        {ERR_NONICKNAMEGIVEN, ":No nickname given"},
        {ERR_ERRONEUSNICKNAME, ":Erroneus nickname"},
        {ERR_NICKNAMEINUSE, ":Nickname is already in use"},
        {ERR_NICKCOLLISION, ":Nickname collision KILL"},
		{ERR_NOTONCHANNEL, ":Not in the channel"},
        {ERR_NOTREGISTERED, ":You have not registered"},
        {ERR_NEEDMOREPARAMS, ":Not enough parameters"},
        {ERR_ALREADYREGISTERED, ":You may not reregister"},
		{ERR_BADCHANNELKEY, ":Cannot join channel (+k)"},
		{ERR_NOPRIVILEGES, ":Permission Denied- You're not an IRC operator"},
		{ERR_CANTKILLSERVER, ":You cant kill a server!"}
};

auto GetErrorMessage(int error, std::string param) -> std::string
{
    auto error_pair = error_map.find(error);

    if (error_pair == error_map.end())
        return "Unknown Error";
    
    if (param != "")
        return std::to_string(error) + " " + param + " " + error_pair->second;
    else
        return std::to_string(error) + " " + error_pair->second;
}
