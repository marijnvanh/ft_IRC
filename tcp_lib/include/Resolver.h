#ifndef __RESOLVER_H__
#define __RESOLVER_H__

#include <netdb.h>
#include <iostream>
namespace TCP
{
    struct AddressInfo
    {
    public:
        AddressInfo(struct addrinfo *address_info);
        ~AddressInfo();
        void Print() const;
        struct addrinfo *GetAddrInfo() const;

        AddressInfo (AddressInfo&& other)
        {
            address_info_ = other.address_info_;
            other.address_info_ = NULL;
        }

        AddressInfo &operator= (AddressInfo&& other)
        {
            address_info_ = other.address_info_;
            other.address_info_ = NULL;
            return (*this);
        }

    private:
        AddressInfo (AddressInfo& other) = delete;
        AddressInfo &operator =(AddressInfo& other) = delete;
        struct addrinfo *address_info_;

    };
} // namespace TCP

namespace TCP::Resolver
{
    TCP::AddressInfo Resolve(const std::string &node, const std::string &service);

    class Error : public std::runtime_error
    {
    public:
        Error(const char* msg) : std::runtime_error(msg) {}
    };
} // namespace TCP::Resolver

#endif
