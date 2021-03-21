#include "Utilities.h"
#include "Numerics.h"

#include <unordered_map>

std::unordered_map<int, std::string> error_map = {
        {ERR_NOTREGISTERED, ":You have not registered"},
        {ERR_NONICKNAMEGIVEN, ":No nickname given"},
        {ERR_NOSUCHNICK, ":No such nick/channel"},
        {ERR_NEEDMOREPARAMS, ":Not enough parameters"},
        {ERR_NOTEXTTOSEND, ":No text to send"},
        {ERR_NOSUCHCHANNEL, ":No such channel"},
		{ERR_BADCHANNELKEY, ":Cannot join channel (+k)"}
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
