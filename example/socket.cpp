#include "socket.h"
#include "coroutine.hpp"
#include "context.h"
#include <source_location>

kiedis::Task<void> echo(kiedis::Socket &socket)
{
    while (true)
    {
        auto [content, ok_read] = co_await socket.read();
        if (!ok_read)
        {
            std::cout<<"Failed to read"<<std::endl;
            co_return;
        }
        auto [size, ok_write] = co_await socket.write(content);
        if (!ok_write)
        {
            std::cout<<"Failed to write"<<std::endl;
            co_return;
        }
    }
}

kiedis::Task<void> accept(kiedis::Socket &socket)
{
    while (true)
    {
        auto [sock_fd, ok] = co_await socket.accept();
        if (!ok)
        {
            std::cout << "failed to accept" << std::endl;
            continue;
        }
        auto ptr = std::make_unique<kiedis::Socket>(socket.get_context(), sock_fd);
        ptr->spawn(echo(*ptr));
    }
}

int main()
{
    kiedis::IOContext ctx;
    kiedis::Socket socket(ctx);
    socket.bind(8080);
    auto t = accept(socket);
    ctx.run();

    return 0;
}