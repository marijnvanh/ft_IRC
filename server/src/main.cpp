#include "RawMessage.h"
#include "Parser.h"
#include <iostream>
#include <memory>
#include <optional>
#include <vector>
#include "AddressInfo.h"
#include "IOController.h"
#include "Message.h"

#include <queue>
#include <unistd.h>
#define PORT "5000"

int main(int argc, char *argv[])
{
    using namespace ft_irc::parser;

    std::cout << "Parsing section" << std::endl;

    std::string source(":emiflake@nixflake PRIVMSG #ft-irc :hello, how are you");
    CharStream cs = CharStream::FromString(source);

    auto message = ft_irc::ParseRawMessage(cs);

    std::cout << "Command: " << message.command.name << std::endl;
    std::cout << "........." << std::endl;

    if (argc != 2)
        exit(1);
    std::string server_address(argv[1]);
    try {
        TCP::AddressInfo address_info(server_address, PORT);
        std::queue<TCP::Message> send_queue;
        std::queue<TCP::Message> read_queue;
        TCP::IOController io_controller(address_info, read_queue, send_queue);
        while (1)
        {
            std::cout << "Poll once" << std::endl;
            io_controller.RunOnce();

            while (read_queue.empty() == false)
            {
                auto message = read_queue.front();
                if (*message.GetData() != "")
                {
                    std::cout << "Received: " << message.GetData() << std::endl;
                    TCP::Message response(message.GetSocket(), "ACK: " + *message.GetData());
                    send_queue.push(std::move(response));
                }
                else
                {
                    if (message.GetSocket()->GetState() == TCP::SocketState::kConnected)
                        std::cout << "New connection received" << std::endl;
                }
                read_queue.pop();
            }
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
