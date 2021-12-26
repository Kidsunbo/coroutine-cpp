#ifndef KIEDIS_AWAITABLE_H
#define KIEDIS_AWAITABLE_H

#include <string>
#include <coroutine>
#include <array>
#include <memory>
#include <queue>
#include <tuple>

#include <iostream>

namespace kiedis
{
    struct ReadFuture
    {
        std::string result;
        std::array<char, 1024> buf;
        std::coroutine_handle<> handle;
        const int fd;
        bool need_suspend = true;
        bool connection_close = false;

        explicit ReadFuture(int fd);
        bool await_ready();
        bool await_suspend(std::coroutine_handle<> h);
        std::tuple<std::string, bool> await_resume();
        bool valid();
    };

    struct WriteFuture
    {
        std::string content;
        std::coroutine_handle<> handle;
        const int fd;
        bool need_suspend = true;
        bool connection_close = false;
        unsigned long total_write = 0;

        WriteFuture(int fd, std::string content);
        bool await_ready();
        bool await_suspend(std::coroutine_handle<> h);
        std::tuple<unsigned long, bool> await_resume();
        bool valid();
    };

    struct AcceptFuture
    {
        std::coroutine_handle<> handle;
        const int fd;
        bool need_suspend = true;
        bool connection_close = false;
        std::queue<int> result;

        explicit AcceptFuture(int fd);
        bool await_ready();
        bool await_suspend(std::coroutine_handle<> h);
        std::tuple<int, bool> await_resume(); //return value: socket_fd, remote ip, remote port, success.
        bool valid();
    };

} // namespace kiedis

#endif