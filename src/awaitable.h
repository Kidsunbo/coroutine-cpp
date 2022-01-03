#ifndef KIEDIS_AWAITABLE_H
#define KIEDIS_AWAITABLE_H

#include <string>
#include <coroutine>
#include <array>
#include <memory>
#include <queue>
#include <tuple>
#include "context.h"
#include "utils.h"



namespace kiedis
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
    };

    struct AcceptFuture
    {
        const int fd;
        const int epoll_fd;
        EventData data = {.state = NextState::In};

        AcceptFuture(int fd, int epoll_fd);
        bool await_ready();
        bool await_suspend(std::coroutine_handle<> h);
        std::tuple<int, bool> await_resume(); // return value: socket_fd, remote ip, remote port, success.

        ~AcceptFuture(){
            std::cout<<"~AcceptFuture"<<std::endl;
        }
    };

} // namespace kiedis

#endif