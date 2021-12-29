#ifndef KIEDIS_AWAITABLE_H
#define KIEDIS_AWAITABLE_H

#include <string>
#include <coroutine>
#include <array>
#include <memory>
#include <queue>
#include <tuple>


namespace kiedis
{
    struct ReadFuture
    {
        std::coroutine_handle<> &handle;
        const int fd;

        explicit ReadFuture(int fd, std::coroutine_handle<> &h);
        bool await_ready();
        bool await_suspend(std::coroutine_handle<> h);
        std::tuple<std::string, bool> await_resume();
    };

    struct WriteFuture
    {
        std::string content;
        std::coroutine_handle<> &handle;
        const int fd;

        WriteFuture(int fd, std::coroutine_handle<> &h, std::string content);
        bool await_ready();
        bool await_suspend(std::coroutine_handle<> h);
        std::tuple<unsigned long, bool> await_resume();
    };

    struct AcceptFuture
    {
        const int fd;
        std::coroutine_handle<> &handle;

        AcceptFuture(int fd, std::coroutine_handle<> &h);
        bool await_ready();
        bool await_suspend(std::coroutine_handle<> h);
        std::tuple<int, bool> await_resume(); // return value: socket_fd, remote ip, remote port, success.
    };

} // namespace kiedis

#endif