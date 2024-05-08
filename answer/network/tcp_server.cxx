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

			int opt = 1;
			if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
				perror("setsockopt");
				exit(EXIT_FAILURE);
			}

            auto ret = bind(socket_fd, (struct sockaddr *)&monitor_addr, addrlen);
            assert(!ret);
            if constexpr (socket_protocol == enum_socket_protocol::tcp) {
                ret = listen(socket_fd, 1024);
                assert(!ret);
                printf("server listening on %s:%d\n", ipaddr.c_str(), port);
            }
        }
#if 0
        setNonBlocking(socket_fd);
        if (socket_protocol == enum_socket_protocol::tcp) { // disable Nagle for TCP sockets.
            disableNagle(socket_fd);
        }
#endif
        return socket_fd;
    }

    net_socket(const std::string& _ipaddr, uint16_t _port):
        ipaddr(_ipaddr), port(_port),
        socketfd(create_socketfd())
    {}
};

struct tcp_server {
private:
    using tcp_server_socket = net_socket<enum_socket_protocol::tcp, enum_socket_type::server>;
    std::vector<tcp_server_socket> all_sockets;
    tcp_server_socket listening_socket;
    constexpr static size_t MAX_EVENTS = 10;

public:
    tcp_server(const std::string& _ipaddr, uint16_t _port):
        listening_socket(_ipaddr, _port)
    {}

    void poll() {
        epoll_event event_list[MAX_EVENTS];

        auto add_new_event = [](int epoll_fd, int new_socket) {
            epoll_event event;
            event.data.fd = new_socket;
            event.events = EPOLLIN | EPOLLET;
            if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_socket, &event) == -1) {
                perror("epoll_ctl: add");
                close(new_socket);
            }
        };

        auto epoll_fd = epoll_create1(0);
        assert(epoll_fd >= 0);

        add_new_event(epoll_fd, listening_socket.socketfd);

        while (true) {
            auto num_events = epoll_wait(epoll_fd, event_list, MAX_EVENTS, -1);
            if (num_events == -1) {
                perror("epoll_wait");
                continue;
            }

            for (int i = 0; i < num_events; ++i) {
                if (event_list[i].data.fd == listening_socket.socketfd) {
                    // accept new connection
                    sockaddr_in monitor_addr {};
                    socklen_t addr_len = sizeof(monitor_addr);
                    auto new_socket = accept(listening_socket.socketfd, (struct sockaddr *)&monitor_addr, &addr_len);
                    assert(new_socket >= 0);

                    // set new_socket as non-blocking
                    fcntl(new_socket, F_SETFL, O_NONBLOCK);
                    add_new_event(epoll_fd, new_socket);
                    std::cout << "New connection from " << inet_ntoa(monitor_addr.sin_addr) << ":" << ntohs(monitor_addr.sin_port) << std::endl;
                } else {
                    // 处理已连接socket的读事件
                    /* handle_read_event(epoll_fd, events[i].data.fd); */
                }
            }
        }
    }
};

int main() {
    std::thread server_thread([]() {
        tcp_server server {"0.0.0.0", 8080};
        server.poll();
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
