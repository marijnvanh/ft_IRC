#ifndef __LOCAL_USER_H__
#define __LOCAL_USER_H__

#include "Client.h"
#include "User.h"
#include "ILocalUser.h"

class LocalUser : public Client, ILocalUser, User
{
    public:

    LocalUser() = delete;
    LocalUser(Client &&old_client) : IClient(std::move(old_client)), Client(std::move(old_client))
    {
        logger.Log(LogLevel::INFO, "Client converted to local user");
    };
    ~LocalUser()
    {
        logger.Log(LogLevel::INFO, "Local User died");
    };

    auto ShouldPing(time_t current_time) -> bool override
	{
		return (current_time >= ping_time_);
	};

    private:
};

#endif
