#ifndef __USER_H__
#define __USER_H__

#include <memory>
#include <string>
#include <unordered_map>

#include "IUser.h"

class User : public virtual IUser 
{
    public:

    virtual ~User();

	auto FormatUserMode(void) const -> std::string;

    auto AddChannel(IChannel *channel) -> void override;
    auto RemoveChannel(const std::string &channel_name) -> void override;
    auto RemoveUserFromAllChannels() -> void override;
	auto GetChannels(void) -> std::unordered_map<std::string, IChannel*>& override;
	auto GenerateNickMessage(const std::string &this_server_name) const -> std::string override;

    private:
    
    std::unordered_map<std::string, IChannel*> channels_;
};

#endif
