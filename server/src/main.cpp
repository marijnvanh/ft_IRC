#include "RawMessage.hpp"
#include "Parser.hpp"
#include "Mutex.hpp"
#include <iostream>
#include <memory>
#include <optional>
#include <vector>
#include <cassert>
#include <thread>
#include <chrono>
#include <queue>
 
#include "AddressInfo.h"
#include "IOController.h"
#include "Message.h"

#include <unistd.h>
#define PORT "5000"

struct Point {
    int x, y;
    Point(int x, int y) : x(x), y(y) {}
};

std::ostream& operator<<(std::ostream& os, const Point& p)
{
    os << "Point(" << p.x << ", " << p.y << ")";
    return os;
}

int main(int argc, char *argv[])
{
    auto mux = ft_irc::MakeMutex<Point>(0, 0);

    std::thread t1([&]() {
        for (;;) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            mux.Access([](Point& p) { 
                std::cout << "Locked mux" << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                p.x++;
                std::cout << "Incremented x" << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            });
            std::cout << "Released mux" << std::endl;
        }
    });

    std::thread t2([&]() {
        for (;;) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::cout << "Current value: " << mux << std::endl;
        }
    });

    t1.join();
    t2.join();

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
