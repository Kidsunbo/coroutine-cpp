#ifndef KIEDIS_CONTEXT_H
#define KIEDIS_CONTEXT_H

/**
 * This file define a class wrapper for epoll to handle the context and file descriptor for the whole library.
 **/

#include <list>
#include <memory>
#include <string>
#include "coroutine.hpp"

namespace kiedis
{
    class Socket;
    class IOContext
    {
        friend class Socket;
        friend void co_spawn(Socket *ptr, Task<void> &&t);
        friend void co_spawn(std::unique_ptr<Socket> ptr, Task<void> &&t);
        friend void co_spawn(Socket &sock, Task<void> &&t);

        const int epoll_fd;
        std::list<std::unique_ptr<Socket>> connections;
        bool stop;
        int timeout;
        std::string last_word;

    public:
        explicit IOContext(int timeout = -1);
        ~IOContext() noexcept;
        IOContext(const IOContext &) = delete;
        IOContext(IOContext &&ctx);
        void run();
        const std::string &get_last_word();

    private:
        void add(Socket *sock);
        void remove(Socket *sock);
    };

    void co_spawn(Socket *ptr, Task<void> &&t);
    void co_spawn(std::unique_ptr<Socket> ptr, Task<void> &&t);
    void co_spawn(Socket &sock, Task<void> &&t);

} // namespace kiedis

#endif