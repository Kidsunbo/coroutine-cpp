#include "socket.h"
#include "coroutine.hpp"
#include "context.h"
#include <iostream>

kiedis::Task<void> echo(kiedis::Socket sock)
{
    auto &ctx = co_await this_core::excutor;
    while (true)
    {
        auto [content, ok_read] = co_await sock.read();
        if (!ok_read)
        {
            kiedis::log("return code:", ok_read);
            co_return;
        }
        auto [len, ok_write] = co_await sock.write(content);
        if (!ok_write)
        {
            kiedis::log("return code:", ok_write);
            co_return;
        }
    }
}

kiedis::Task<void> listen(kiedis::Socket sock)
{
    auto &ctx = co_await this_core::excutor;
    while (true)
    {
        auto [sock_fd, ok] = co_await sock.accept();
        if (!ok)
        {
            co_return;
        }
        kiedis::co_spawn(ctx, echo(kiedis::Socket{ctx,sock_fd}));
    }
}

int main()
{
    kiedis::IOContext ctx;
    kiedis::Socket socket(ctx);
    socket.bind(8080);
    kiedis::co_spawn(ctx, listen(std::move(socket)));
    ctx.run();

    return 0;
}