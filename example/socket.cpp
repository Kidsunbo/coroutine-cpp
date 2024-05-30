#include "socket.h"
#include "coroutine.hpp"
#include "context.h"
#include <iostream>

co_cpp::Task<void> echo(co_cpp::Socket sock)
{
    auto &ctx = co_await this_core::excutor;
    while (true)
    {
        auto [content, ok_read] = co_await sock.read();
        if (!ok_read)
        {
            co_cpp::log("return code:", ok_read);
            co_return;
        }
        auto [len, ok_write] = co_await sock.write(content);
        if (!ok_write)
        {
            co_cpp::log("return code:", ok_write);
            co_return;
        }
    }
}

co_cpp::Task<void> listen(co_cpp::Socket sock)
{
    auto &ctx = co_await this_core::excutor;
    while (true)
    {
        auto [sock_fd, ok] = co_await sock.accept();
        if (!ok)
        {
            co_return;
        }
        co_cpp::co_spawn(ctx, echo(co_cpp::Socket{ctx,sock_fd}));
    }
}

int main()
{
    co_cpp::IOContext ctx;
    co_cpp::Socket socket(ctx);
    socket.bind(8080);
    co_cpp::co_spawn(ctx, listen(std::move(socket)));
    ctx.run();

    return 0;
}