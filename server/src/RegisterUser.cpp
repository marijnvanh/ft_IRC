#include "RegisterUser.h"

auto RegisterUser(std::shared_ptr<IClientDatabase> client_database, std::shared_ptr<IRC::Mutex<IClient>> client) -> void
{
    {
        auto client_handle = client->Take();

        if (client_handle->GetState() != IClient::State::kUnRegistered)
            return ; //TODO double check this returns the whole function

        if (client_handle->GetNickname() == "" || client_handle->GetUsername() == "")
            return ;

        client_handle->SetState(IClient::State::kRegistered);
        client_handle->SetType(IClient::Type::kLocalUser); //TODO double check we only use this for local users
    }
    (void)client_database;
    //TODO inform all clients of new user
}
