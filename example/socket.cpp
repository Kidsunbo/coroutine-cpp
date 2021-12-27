#include "socket.h"
#include "coroutine.hpp"
#include "context.h"

kiedis::Task<void> accept(kiedis::Socket &socket)
{
    while (true)
    {
        auto [sock_fd, ok] = co_await socket.accept();
        std::cout << ok << std::endl;
    }
}

int main()
{
    kiedis::IOContext ctx;
    kiedis::Socket socket(ctx);
    socket.bind(8080);
    accept(socket);
    ctx.run();

    return 0;
}