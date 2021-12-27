#include "socket.h"
#include "coroutine.hpp"


kiedis::Task<void> accept(kiedis::Socket& socket){
    while(true){
        auto [sock_fd, ok] = co_await socket.accept();
        std::cout<<ok<<std::endl;
    }
}

int main(){



}