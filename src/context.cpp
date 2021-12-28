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
        while (!stop)
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
                if (events[i].events & EPOLLIN)
                {
                    if(sock->is_server()){
                        sock->resume_accpet();
                    }else{
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

    void IOContext::add(std::unique_ptr<Socket> sock)
    {

    }

    void IOContext::remove(Socket *sock)
    {
    }

} // namespace kiedis
