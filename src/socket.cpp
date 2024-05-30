#include "socket.h"
#include "utils.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include "context.h"

namespace co_cpp
{

    Socket::Socket(IOContext &ctx) : ctx(ctx)
    {
    }

    Socket::Socket(IOContext &ctx, int socket_fd) : ctx(ctx), socket_fd(socket_fd) {}

    Socket::Socket(Socket &&sock) : ctx(sock.ctx), socket_fd(sock.socket_fd)
    {
        sock.socket_fd = 0;
    }

    Socket::~Socket() noexcept
    {
        if (socket_fd != 0)
            close(socket_fd);
        log("【KIE】");
    }

    bool Socket::connect(std::string_view ip, unsigned short port)
    {
        if (socket_fd != 0)
        {
            return false;
        }

        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_port = htons(port);

        if (inet_pton(AF_INET, ip.data(), &address.sin_addr) <= 0)
        {
            return false;
        }

        if (socket_fd = socket(AF_INET, SOCK_STREAM, 0); socket_fd < 0)
        {
            return false;
        }

        if (::connect(socket_fd, reinterpret_cast<sockaddr *>(&address), sizeof(address)) < 0)
        {
            return false;
        }

        set_non_blocking(socket_fd);

        return true;
    }
    bool Socket::bind(unsigned short port, int listen_max)
    {
        if (socket_fd != 0)
        {
            return false;
        }

        if (socket_fd = socket(AF_INET, SOCK_STREAM, 0); socket_fd < 0)
        {
            return false;
        }

        int opt;
        if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
        {
            return false;
        }
        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);
        if (::bind(socket_fd, reinterpret_cast<sockaddr *>(&address), sizeof(address)) < 0)
        {
            return false;
        }

        if (listen(socket_fd, listen_max) < 0)
        {
            return false;
        }

        set_non_blocking(socket_fd);

        return true;
    }

    AcceptFuture Socket::accept()
    {
        return AcceptFuture{socket_fd, ctx.get_epoll_fd()};
    }

    ReadFuture Socket::read()
    {
        return ReadFuture{socket_fd, ctx.get_epoll_fd()};
    }

    WriteFuture Socket::write(std::string content)
    {
        return WriteFuture{socket_fd, std::move(content), ctx.get_epoll_fd()};
    }

} // namespace co_cpp
