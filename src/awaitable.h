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
        bool connection_close = false;

        explicit ReadFuture(int fd, std::coroutine_handle<> &h);
        bool await_ready();
        bool await_suspend(std::coroutine_handle<> h);
        std::tuple<std::string, bool> await_resume();
        bool valid();
    };

    struct WriteFuture
    {
        std::string content;
        std::coroutine_handle<> &handle;
        const int fd;
        bool connection_close = false;

        WriteFuture(int fd, std::coroutine_handle<> &h, std::string content);
        bool await_ready();
        bool await_suspend(std::coroutine_handle<> h);
        std::tuple<unsigned long, bool> await_resume();
        bool valid();
    };

    struct AcceptFuture
    {
        const int fd;
        std::coroutine_handle<> &handle;
        bool connection_close = false;

        AcceptFuture(int fd, std::coroutine_handle<> &h);
        bool await_ready();
        bool await_suspend(std::coroutine_handle<> h);
        std::tuple<int, bool> await_resume(); // return value: socket_fd, remote ip, remote port, success.
        bool valid();
    };

    struct Awaitable
    {
        bool await_ready()
        {
            return false;
        }
        bool await_suspend(std::coroutine_handle<> h)
        {
            return true;
        }
        int await_resume()
        {
            return 10;
        }
    };

} // namespace kiedis

#endif