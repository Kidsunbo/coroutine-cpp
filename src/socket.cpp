#include "socket.h"
#include "utils.h"
#include <netinet/in.h>
#include <arpa/inet.h>

namespace kiedis
{

    void Socket::resume_read()
    {
        if (!read_co_handle.done())
        {
            read_co_handle.resume();
        }
    }

    void Socket::resume_write()
    {
        if (!write_co_handle.done())
        {
            write_co_handle.resume();
        }
    }

    Socket::Socket(IOContext &ctx) : Socket(ctx, 0)
    {
    }

    Socket::Socket(IOContext &ctx, int fd) : ctx(ctx), socket_fd(fd), read_co_handle(std::noop_coroutine()), write_co_handle(std::noop_coroutine())
    {
        set_non_blocking(socket_fd);
    }

    Socket::Socket(Socket &&socket) : ctx(socket.ctx), socket_fd(socket.socket_fd), read_co_handle(socket.read_co_handle), write_co_handle(socket.write_co_handle)
    {
        socket.socket_fd = 0;
    }

    Socket::~Socket() noexcept
    {
        // No need to destroy read_co_handle and write_co_handle because it's lifetime is handled by the Task of the coroutine state. Or double free will occur.
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

        if (::connect(socket_fd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0)
        {
            return false;
        }

        return true;
    }
    bool Socket::bind(unsigned short port, int listen_max)
    {
        if (socket_fd != 0)
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

        return true;
    }

    AcceptFuture Socket::accept()
    {
        return AcceptFuture{socket_fd};
    }
    ReadFuture Socket::read()
    {
        return ReadFuture{socket_fd, read_co_handle};
    }
    WriteFuture Socket::write(const std::string &content)
    {
        return WriteFuture{socket_fd, write_co_handle, content};
    }

} // namespace kiedis
