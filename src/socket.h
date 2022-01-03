#ifndef KIEDIS_SOCKET_H
#define KIEDIS_SOCKET_H

#include <string>
#include "awaitable.h"
#include "utils.h"


namespace kiedis
{

    class IOContext;

    class Socket {
        int socket_fd;
        IOContext& ctx;


        public:
        Socket(IOContext& ctx);
        Socket(IOContext& ctx, int socket_fd);
        Socket(Socket&& sock);
        ~Socket();

        bool connect(std::string_view ip, unsigned short port);
        bool bind(unsigned short port, int listen_max = 1024);
        
        AcceptFuture accept();
        ReadFuture read();
        WriteFuture write(std::string content);
    };
} // namespace kiedis

#endif