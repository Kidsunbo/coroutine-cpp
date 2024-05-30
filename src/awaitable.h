#ifndef COROUTINECPP_AWAITABLE_H
#define COROUTINECPP_AWAITABLE_H

#include <string>
#include <coroutine>
#include <array>
#include <memory>
#include <queue>
#include <tuple>
#include "context.h"
#include "utils.h"



namespace co_cpp
{



    struct ReadFuture
    {
        const int fd;
        const int epoll_fd;
        EventData data = {.state = NextState::In};

        explicit ReadFuture(int fd, int epoll_fd);
        bool await_ready();
        bool await_suspend(std::coroutine_handle<> h);
        std::tuple<std::string, bool> await_resume();
        ~ReadFuture() noexcept;
    };

    struct WriteFuture
    {
        std::string content;
        const int fd;
        const int epoll_fd;
        EventData data = {.state = NextState::Out};

        WriteFuture(int fd, std::string content, int epoll_fd);
        bool await_ready();
        bool await_suspend(std::coroutine_handle<> h);
        std::tuple<unsigned long, bool> await_resume();
        ~WriteFuture() noexcept;
    };

    struct AcceptFuture
    {
        const int fd;
        const int epoll_fd;
        EventData data = {.state = NextState::In};

        AcceptFuture(int fd, int epoll_fd);
        bool await_ready();
        bool await_suspend(std::coroutine_handle<> h);
        std::tuple<int, bool> await_resume();
        ~AcceptFuture() noexcept;
    };

} // namespace co_cpp

#endif