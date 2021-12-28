#include "awaitable.h"
#include "coroutine.hpp"

#include <iostream>


int main()
{
    std::coroutine_handle<> h;
    std::cout<<std::boolalpha<<bool(h)<<std::endl;

    h = std::noop_coroutine();
    std::cout<<std::boolalpha<<bool(h)<<std::endl;
    std::cout<<std::boolalpha<<h.done()<<std::endl;

    return 0;
}