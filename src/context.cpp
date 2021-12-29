#include "context.h"
#include "socket.h"

#include <sys/epoll.h>
#include <array>
#include <cstring>

namespace kiedis
{
    IOContext::IOContext(int timeout) : epoll_fd(epoll_create1(0)), stop(false), timeout(timeout)
    {
    }

    IOContext::IOContext(IOContext &&ctx) : epoll_fd(ctx.epoll_fd), stop(ctx.stop), timeout(ctx.timeout), connections(std::move(ctx.connections)), last_word(std::move(ctx.last_word))
    {
    }

    IOContext::~IOContext() noexcept
    {
    }

    void IOContext::run()
    {
        std::array<epoll_event, 256> events;
        for(int i=0;i<10;i++)
        {
            auto fd_count = epoll_wait(this->epoll_fd, events.data(), events.size(), timeout);
            if (fd_count == -1)
            {
                last_word = strerror(errno);
                return;
            }
            for (std::size_t i = 0; i < fd_count; i++)
            {
                auto sock = static_cast<Socket *>(events[i].data.ptr);
                if(events[i].events & EPOLLHUP || events[i].events & EPOLLERR){
                    close(sock->socket_fd);
                    remove(sock);
                    continue;
                }
                if (events[i].events & EPOLLIN)
                {
                    if (sock->is_server())
                    {
                        sock->resume_accpet();
                    }
                    else
                    {
                        sock->resume_read();
                    }
                }
                if (events[i].events & EPOLLOUT)
                {
                    sock->resume_write();
                }
            }
        }
    }

    const std::string &IOContext::get_last_word()
    {
        return last_word;
    }

    void IOContext::add(Socket *sock)
    {
        epoll_event ev{};
        ev.data.ptr = sock;
        ev.events = EPOLLIN;
        if (auto ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock->socket_fd, &ev); ret < 0)
        {
            std::terminate();
        }
        connections.push_back(std::unique_ptr<Socket>(sock));
    }

    void IOContext::remove(Socket *sock)
    {
        for (auto iter = connections.begin(); iter != connections.end(); iter++)
        {
            if (iter->get() == sock)
            {
                connections.erase(iter);
                break;
            }
        }
    }

    void co_spawn(Socket *ptr, Task<void> &&t)
    {
        ptr->long_live_task = std::move(t);
        ptr->get_context().add(ptr);
    }

    void co_spawn(std::unique_ptr<Socket> ptr, Task<void> &&t)
    {
        co_spawn(ptr.release(), std::move(t));
    }

    void co_spawn(Socket &sock, Task<void> &&t)
    {
        sock.long_live_task = std::move(t);
        epoll_event ev{};
        ev.data.ptr = &sock;
        ev.events = EPOLLIN;
        if (auto ret = epoll_ctl(sock.get_context().epoll_fd, EPOLL_CTL_ADD, sock.socket_fd, &ev); ret < 0)
        {
            std::terminate();
        }
    }

} // namespace kiedis
