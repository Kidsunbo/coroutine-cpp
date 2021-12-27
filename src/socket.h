#ifndef KIEDIS_SOCKET_H
#define KIEDIS_SOCKET_H

#include <string>
#include "coroutine.hpp"
#include "awaitable.h"

namespace kiedis
{

    class IOContext;

    class Socket {
        int socket_fd;
        IOContext& ctx;
        std::coroutine_handle<> read_co_handle;
        std::coroutine_handle<> write_co_handle;

        public:
        
        explicit Socket(IOContext& ctx);
        Socket(IOContext& ctx, int fd);
        Socket(Socket&& socket);
        ~Socket()noexcept;

        bool connect(std::string_view ip, unsigned short port);
        bool bind(unsigned short port, int listen_max = 1024);

        AcceptFuture accept();
        ReadFuture read();
        WriteFuture write(const std::string& content);

        void resume_read();
        void resume_write();
        
    };
} // namespace kiedis

#endif