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
        std::string remote_ip;
        unsigned int port;
        IOContext& ctx;
        std::coroutine_handle<> read_co_handle;
        std::coroutine_handle<> write_co_handle;

        public:
        
        explicit Socket(IOContext& ctx);
        Socket(IOContext& ctx, int fd);
        Socket(Socket&& socket);
        ~Socket()noexcept;

        bool connect(std::string_view ip, unsigned int port);
        bool bind(unsigned int port);

        AcceptFuture accept();
        ReadFuture read();
        WriteFuture write();

        void resume_read();
        void resume_write();
        
    };
} // namespace kiedis

#endif