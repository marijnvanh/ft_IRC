#ifndef __RESOLVER_H__
#define __RESOLVER_H__

#include <netdb.h>

namespace TCP::Resolver
{
    addrinfo    *Resolve(const std::string &node, const std::string &service);
    void        PrintAddressInfo(struct addrinfo *addr_info);
} // namespace TCP::Resolver

#endif
