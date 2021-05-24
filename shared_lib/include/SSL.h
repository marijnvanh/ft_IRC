#ifndef __TLS_SOCKET_H__
#define __TLS_SOCKET_H__

#ifdef ENABLE_SSL
#include "openssl/ssl.h"
#include <stdexcept>
#include <string>

namespace IRC::TCP
{
    extern SSL_CTX* ctx_g;

    auto InitSSL(const std::string &crt, const std::string &key) -> void;
    
    class SSLError : public std::runtime_error
    {
    public:
        SSLError(const char *msg) : std::runtime_error(msg) {}
    };
}

#endif // ENABLE SSL
#endif // __TLS_SOCKET_H__
