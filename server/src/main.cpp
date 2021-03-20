#include "IRCServer.h"

#include <queue>
#include <memory>
#include <unistd.h>
#define PORT "5000"

using namespace IRC;

int main(int argc, char *argv[])
{
    try
	{
		std::string config_path("");
		if (argc >= 2)
			config_path.assign(argv[1]);

        IRCServer server(config_path);

		server.Start();

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
