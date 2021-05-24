#include "SSL.h"
#ifdef ENABLE_SSL
#include "openssl/bio.h"
#include "openssl/err.h"
#include "signal.h"

using namespace IRC;

SSL_CTX* TCP::ctx_g;

/**
 * @brief Init SSL and load certificates and private key
 * @exception SSLError
 */
auto TCP::InitSSL(const std::string &crt, const std::string &key) -> void
{
    static bool init_once = false;

    if (init_once == true)
        return ;
    else
        init_once = true;

    /* Initialize OpenSSL */
    SSL_load_error_strings();
    ERR_load_BIO_strings();
    OpenSSL_add_all_algorithms();
    TCP::ctx_g = SSL_CTX_new(TLS_server_method());

    if (!ctx_g) {
        ERR_print_errors_fp(stderr);
        throw TCP::SSLError("Failed to set up CTX");
    }

    if (SSL_CTX_use_certificate_file(ctx_g, crt.c_str(), SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        throw TCP::SSLError("Failed to load certificate file");
    }

    if (SSL_CTX_use_PrivateKey_file(ctx_g, key.c_str(), SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        throw TCP::SSLError("Failed to load private key file");
    }

    if (!SSL_CTX_check_private_key(ctx_g)) {
        throw TCP::SSLError("Private key does not match the certificate public key");
    }
    /* Ignore SIGPIPE errors (sockets will then return EPIPE instead of a signal SIGPIPE) */
    signal(SIGPIPE, SIG_IGN);

}
#endif // ENABLE SSL