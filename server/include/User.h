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

    auto AddChannel(IChannel *channel) -> void override;
    auto RemoveChannel(const std::string &channel_name) -> void override;
    auto RemoveUserFromAllChannels() -> void override;

    private:
    
    std::unordered_map<std::string, IChannel*> channels_;
};

#endif
