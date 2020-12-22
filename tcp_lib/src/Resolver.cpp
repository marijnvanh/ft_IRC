#include <iostream>
#include "Resolver.h"

#include <netdb.h>
#include <arpa/inet.h>
#include <iostream>

/**
 * @brief Print the addr_info linked list
 * 
 * @param addr_info linked list of addr_info
 */
void TCP::Resolver::PrintAddressInfo(struct addrinfo *addr_info)
{
    struct addrinfo *addr_info_i;
    char ipstr[INET6_ADDRSTRLEN];

    for (addr_info_i = addr_info; addr_info_i != NULL; addr_info_i = addr_info_i->ai_next)
    {
        void *address;
        const char *ip_version;

        // get the pointer to the address itself,
        // different fields in IPv4 and IPv6:
        if (addr_info_i->ai_family == AF_INET)
        { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)addr_info_i->ai_addr;
            address = &(ipv4->sin_addr);
            ip_version = "IPv4";
        }
        else
        { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)addr_info_i->ai_addr;
            address = &(ipv6->sin6_addr);
            ip_version = "IPv6";
        }

        // convert the IP to a string and print it:
        inet_ntop(addr_info_i->ai_family, address, ipstr, sizeof(ipstr));
        printf("  %s: %s\n", ip_version, ipstr);
    }
}

/**
 * @brief Resolve a host name or IP string 
 * 
 * To resolve a local IP for server sockets provide an empty string for the node parameter
 * @param node e.g. "www.example.com" or IP
 * 
 * @param service e.g. "http" or port number
 * @return addrinfo* list of addrinfo
 */
addrinfo *TCP::Resolver::Resolve(const std::string &node, const std::string &service)
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
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error)); //TODO make proper error logging
        //TODO throw error
        return NULL;
    }

    return result;
}

//TODO Create a data structure that encapsulates the addr info data so it can be unique pointer
// freeaddrinfo(result);