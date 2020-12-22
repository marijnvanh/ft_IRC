#ifndef __SOCKET_H__
#define __SOCKET_H__

#include "Resolver.h"

namespace TCP
{

    class Socket
    {
    public:
        Socket(TCP::AddressInfo &&address_info);
    };

} // namespace TCP

#endif