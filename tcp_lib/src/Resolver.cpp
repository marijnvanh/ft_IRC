#include <iostream>
#include "Resolver.h"

#include <netdb.h>
#include <arpa/inet.h>
#include <iostream>
#include <string.h>

TCP::AddressInfo::AddressInfo(struct addrinfo *address_info) : address_info_(address_info)
{}

TCP::AddressInfo::~AddressInfo()
{
    freeaddrinfo(address_info_);
}

struct addrinfo *TCP::AddressInfo::GetAddrInfo() const
{
    return address_info_;
}

/**
 * @brief Print the addr_info linked list
 * 
 * @param addr_info linked list of addr_info
 */
void TCP::AddressInfo::Print() const
{
    char ipstr[INET6_ADDRSTRLEN];

    for (struct addrinfo *addr_info_i = address_info_; addr_info_i != NULL; addr_info_i = addr_info_i->ai_next)
    {
        void *address;
        const char *ip_version;

        if (addr_info_i->ai_family == AF_INET) // IPv4
        { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)addr_info_i->ai_addr;
            address = &(ipv4->sin_addr);
            ip_version = "IPv4";
        }
        else // IPv6
        {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)addr_info_i->ai_addr;
            address = &(ipv6->sin6_addr);
            ip_version = "IPv6";
        }

        inet_ntop(addr_info_i->ai_family, address, ipstr, sizeof(ipstr));
        printf("  %s: %s\n", ip_version, ipstr);
    }
}

/**
 * @brief Resolve a host name or IP string 
 * 
 * To resolve a local IP for server sockets provide an empty string for the node parameter
 * @param node e.g. "www.example.com" or IP
 * @param service e.g. "http" or port number
 * 
 * @exception TCP::Resolver::Error
 * @return addrinfo* list of addrinfo
 */
TCP::AddressInfo TCP::Resolver::Resolve(const std::string &node, const std::string &service)
{
    struct addrinfo hints;
    struct addrinfo *result;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;        // family is UNSPEC so we support IPv4 and IPv6
    hints.ai_socktype = SOCK_STREAM;    // TCP
    if (node == "")
        hints.ai_flags = AI_PASSIVE;    // Enable for local IP (server sockets)

    int error = getaddrinfo(node.c_str(), service.c_str(), &hints, &result);
    if (error != 0)
        throw TCP::Resolver::Error(gai_strerror(error));

    return AddressInfo(result);
}
