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


enum class enum_socket_protocol { udp, tcp };

struct client_socket_tag {};
struct server_socket_tag {};

template<enum_socket_protocol socket_protocol, typename socket_type>
struct net_socket {
    std::string ipaddr;
    uint16_t port;
    int socketfd;

    void setup_socketfd(int socket_fd, client_socket_tag) {
        const addrinfo hints {
            0, AF_INET, 
            socket_protocol == enum_socket_protocol::udp ? SOCK_DGRAM : SOCK_STREAM,
            socket_protocol == enum_socket_protocol::udp ? IPPROTO_UDP : IPPROTO_TCP,
            0, 0, nullptr, nullptr
        };

        addrinfo *result = nullptr;
        auto ret = getaddrinfo(
                ipaddr.c_str(),
                std::to_string(port).c_str(),
                &hints,
                &result);
        assert(!ret);
        for (auto *cur = result; cur; cur = cur->ai_next) {
            ret = connect(socket_fd, cur->ai_addr, cur->ai_addrlen);
            assert(!ret);
            break;
        }
    }

    void setup_socketfd(int socket_fd, server_socket_tag) {
        const sockaddr_in monitor_addr {
            AF_INET,
            htons(port),
            { inet_addr(ipaddr.c_str()) }, {}
        };
        socklen_t addrlen = sizeof (monitor_addr);

        // set socket reusable in case of error when launching application multiple times in short time
        int opt = 1;
        auto ret = setsockopt(socket_fd, SOL_SOCKET, 
                SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
        assert(!ret);

        ret = bind(socket_fd, (struct sockaddr *)&monitor_addr, addrlen);
        assert(!ret);

        if constexpr (socket_protocol == enum_socket_protocol::tcp) {
            ret = listen(socket_fd, 1024);
            assert(!ret);
            std::cout << "server listening on " << ipaddr << ":" << port << std::endl;
        }
    }

    int create_socketfd() {
        auto socket_fd = socket(AF_INET, static_cast<int>(socket_protocol), 0);
        setup_socketfd(socket_fd, socket_type {});
#if 0
        setNonBlocking(socket_fd);
        if constexpr (socket_protocol == enum_socket_protocol::tcp) { // disable Nagle for TCP sockets.
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
    using tcp_server_socket = net_socket<enum_socket_protocol::tcp, server_socket_tag>;
    tcp_server_socket listening_socket;
    constexpr static size_t MAX_EVENTS = 10, BUFFER_SIZE = 1024;

public:
    tcp_server(const std::string& _ipaddr, uint16_t _port):
        listening_socket(_ipaddr, _port)
    {}

	void handle_read_event(int epoll_fd, int fd) {
        // data buffer
        char buf[BUFFER_SIZE];
        struct iovec iov[1];
        iov[0].iov_base = buf;
        iov[0].iov_len = sizeof(buf);

        // ancillary data
        union {
            struct cmsghdr cmh;
            char control[CMSG_SPACE(sizeof(int))];
        } control_un;

        // message header
        struct msghdr msg {
            nullptr, 0,
            iov, 1,
            control_un.control,
            sizeof (control_un.control),
            0
        };

        ssize_t bytes_received = recvmsg(fd, &msg, MSG_DONTWAIT);
        assert(bytes_received > 0); 

        std::cout << "server received: " << std::string(buf, bytes_received) << std::endl;

        const std::string m = "bye :)";
        send(fd, m.c_str(), m.size(), 0);

#if 0
        // check ancillary data
        for (auto *cmptr = CMSG_FIRSTHDR(&msg); cmptr != NULL; cmptr = CMSG_NXTHDR(&msg, cmptr)) {
            if (cmptr->cmsg_level == SOL_SOCKET && cmptr->cmsg_type == SCM_RIGHTS) {
                // 处理接收到的文件描述符（或其他类型的辅助数据）
                int *fdptr = (int *) CMSG_DATA(cmptr);
                std::cout << "Received file descriptor: " << *fdptr << std::endl;
            }
        }
#endif
	}

    void poll() {
        epoll_event event_list[MAX_EVENTS];

        const auto add_new_event = [](int epoll_fd, int new_socket, uint32_t event_mask) {
            epoll_event event;
            event.data.fd = new_socket;
            event.events = event_mask;
            if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_socket, &event) == -1) {
                perror("ERROR: epoll_ctl: add");
                close(new_socket);
            }
        };

        const auto epoll_fd = epoll_create1(0);
        assert(epoll_fd >= 0);

        // listen socket fd set LT mode
        add_new_event(epoll_fd, listening_socket.socketfd, EPOLLIN);

        while (true) {
            auto num_events = epoll_wait(epoll_fd, event_list, MAX_EVENTS, -1);
            if (num_events == -1) {
                perror("ERROR: epoll_wait");
                continue;
            }

            for (int i = 0; i < num_events; i ++ ) {
                if (event_list[i].data.fd == listening_socket.socketfd) {
                    // accept new connection
                    sockaddr_in client_addr {};
                    socklen_t addr_len = sizeof(client_addr);
                    auto new_socket = accept(listening_socket.socketfd, (struct sockaddr *)&client_addr, &addr_len);
                    assert(new_socket >= 0);

                    // set new_socket as non-blocking
                    // as we want data socketfd set as ET mode
                    fcntl(new_socket, F_SETFL, O_NONBLOCK);
                    add_new_event(epoll_fd, new_socket, EPOLLIN | EPOLLET);
                    std::cout << "New connection from " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << std::endl;
                } else {
                    handle_read_event(epoll_fd, event_list[i].data.fd);
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
        net_socket<enum_socket_protocol::tcp, client_socket_tag>
            tcp_client_socket {"127.0.0.1", 8080, };
        const std::string msg = "hello world";
        char buf[1024];
        send(tcp_client_socket.socketfd, msg.c_str(), msg.size(), 0);
        auto len = recv(tcp_client_socket.socketfd, buf, 1024, 0);
        std::cout << "client received: " << std::string(buf, len) << std::endl;
    });

    server_thread.join();

    client_thread.join();
    return 0;
}
