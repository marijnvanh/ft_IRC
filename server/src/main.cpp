#include "IRCServer.h"

#include <queue>
#include <memory>
#include <unistd.h>
#define PORT "5000"

using namespace IRC;

int main(int argc, char *argv[])
{
    if (argc != 2)
        exit(1);

    std::string server_address(argv[1]);

    try
	{
        IRCServer server;

		server.Start(std::move(server_address));

		while (1)
        {
			server.RunOnce();

            sleep(1);
        }
    }
    catch (TCP::AddressInfo::ResolveError &ex)
    {
        std::cout << "Resolve error: " << ex.what() << std::endl;
        exit(-1);
    }
    catch (TCP::Socket::Error &ex)
    {
        std::cout << "Socket error: " << ex.what() << std::endl;
        exit(-1);
    }

    return (0);
}
