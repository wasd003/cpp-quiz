#include <cassert>
#include <thread>
#include <cstdint>
#include <quiz/base.h>
#include <sstream>
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <fcntl.h>


enum class enum_socket_protocol { udp = SOCK_DGRAM, tcp = SOCK_STREAM, };
enum class enum_socket_type { client, server, };

template<enum_socket_protocol socket_protocol, enum_socket_type socket_type>
struct net_socket {
private:
    std::string ipaddr;
    uint16_t port;
    int socketfd;

    int create_socketfd() {
        auto socket_fd = socket(AF_INET, static_cast<int>(socket_protocol), 0);

        if constexpr (socket_type ==  enum_socket_type::client) {
            const addrinfo hints {0, AF_INET, static_cast<int>(socket_protocol),
                socket_protocol == enum_socket_protocol::udp ? IPPROTO_UDP : IPPROTO_TCP, 0, 0, nullptr, nullptr};

            addrinfo *result = nullptr;
            auto ret = getaddrinfo(
                    ipaddr.c_str(),
                    std::to_string(port).c_str(),
                    &hints,
                    &result);
            assert(!ret);
            for (addrinfo *cur = result; cur; cur = cur->ai_next) {
                ret = connect(socket_fd, cur->ai_addr, cur->ai_addrlen);
                assert(!ret);
                break;
            }
        } else {
            const sockaddr_in monitor_addr {
                AF_INET,htons(port),
                { inet_addr(ipaddr.c_str()) }, {}
            };
            socklen_t addrlen = sizeof (monitor_addr);
            auto ret = bind(socket_fd, (struct sockaddr *)&monitor_addr, addrlen);
            assert(!ret);
            if constexpr (socket_protocol == enum_socket_protocol::tcp) {
                ret = listen(socket_fd, 1024);
                assert(!ret);
                printf("server listening on %s:%d\n", ipaddr.c_str(), port);
            }

            auto new_socket = accept(socket_fd, (struct sockaddr *)&monitor_addr, &addrlen);
            UNUSED(new_socket);
            printf("recv connection\n");
        }
#if 0
        setNonBlocking(socket_fd);
        if (socket_protocol == enum_socket_protocol::tcp) { // disable Nagle for TCP sockets.
            disableNagle(socket_fd);
        }
#endif
        return socket_fd;
    }

public:
    net_socket(const std::string& _ipaddr, uint16_t _port):
        ipaddr(_ipaddr), port(_port),
        socketfd(create_socketfd())
    {}
};

#if 0
struct tcp_server {
private:
    std::vector<tcp_socket> all_sockets;
    tcp_socket listening_socket;

public:
    tcp_server() {

    }
};
#endif

int main() {
    std::thread server_thread([]() {
        net_socket<enum_socket_protocol::tcp, enum_socket_type::server>
            tcp_server_socket {"0.0.0.0", 8080};
    });

    sleep(1);
    std::thread client_thread([]() {
        net_socket<enum_socket_protocol::tcp, enum_socket_type::client>
            tcp_client_socket {"127.0.0.1", 8080, };
    });

    server_thread.join();

    client_thread.join();
    return 0;
}
