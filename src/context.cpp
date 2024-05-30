#include "context.h"
#include <sys/epoll.h>
#include <array>
#include <list>

#include "coroutine.hpp"

#include <iostream>

namespace co_cpp
{

    IOContext::IOContext()
    {
        if (epoll_fd = epoll_create1(0); epoll_fd < 0)
        {
            std::exit(1);
        }
    }

    IOContext::~IOContext() noexcept {}

    void IOContext::run()
    {
        std::array<epoll_event, 256> events;
        while (true)
        {
            auto fd_count = epoll_wait(this->epoll_fd, events.data(), events.size(), -1);
            if (fd_count == -1)
            {
                std::exit(2);
            }
            for (std::size_t i = 0; i < fd_count; i++)
            {
                auto t = static_cast<EventData *>(events[i].data.ptr);
                if (events[i].events & EPOLLERR || events[i].events & EPOLLHUP)
                {
                    log(t, t->h.address());
                    t->state = NextState::Error;
                    t->h.resume();
                }
                else if (events[i].events & EPOLLIN)
                {
                    log(t, t->h.address());
                    t->state = NextState::In;
                    t->h.resume();
                }
                else if (events[i].events & EPOLLOUT)
                {
                    log(t, t->h.address());
                    t->state = NextState::Out;
                    t->h.resume();
                }
            }
        }
    }

    int IOContext::get_epoll_fd()
    {
        return epoll_fd;
    }

    void remove_from_context(IOContext &ctx, std::coroutine_handle<> h)
    {
        ctx.tasks.erase(h);
    }

    void add_to_context(IOContext &ctx, Task<void> &&t)
    {
        auto co_handle = t.co_handle;
        ctx.tasks.insert({co_handle, std::make_unique<Task<void>>(std::move(t))});
    }

    void co_spawn(IOContext &ctx, Task<void> &&t)
    {
        t.co_handle.promise().set_context(ctx);
        auto co_handle = t.co_handle;
        add_to_context(ctx, std::move(t));
        co_handle.resume();
    }

} // namespace co_cpp
