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
        void Print();
        AddressInfo(AddressInfo&& other)
        {
            (void)other;
            std::cout << "hello" << std::endl;
        }
        AddressInfo& operator=(const AddressInfo&& other)
        {
            std::cout << "hello" << std::endl;
            (void)other;
            return *this;
        };
        AddressInfo(const AddressInfo& other){
            std::cout << "hello" << std::endl;
            (void)other;
            };
        AddressInfo& operator=(const AddressInfo& other){
            std::cout << "hello" << std::endl;
            (void)other; 
        return *this;
        };

    private:
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
