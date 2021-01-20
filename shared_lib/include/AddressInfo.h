#ifndef __ADDRESS_INFO_H__
#define __ADDRESS_INFO_H__

#include <netdb.h>
#include <iostream>
namespace IRC::TCP
{
    struct AddressInfo
    {
    private:
        struct addrinfo *address_info_;
    public:
        AddressInfo(const std::string &node, const std::string &service);
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

        AddressInfo (AddressInfo& other) = delete;
        AddressInfo &operator =(AddressInfo& other) = delete;

        class ResolveError : public std::runtime_error
        {
        public:
            ResolveError(const char* msg) : std::runtime_error(msg) {}
        };

    };
} // namespace IRC::TCP

#endif
