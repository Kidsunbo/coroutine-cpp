#include "socket.h"
#include "coroutine.hpp"
#include "context.h"

kiedis::Task<void> accept(kiedis::Socket &socket)
{
    while (true)
    {
        auto [sock_fd, ok] = co_await socket.accept();
        if(!ok){
            std::cout<<"failed to accept"<<std::endl;
            continue;
        }
        
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