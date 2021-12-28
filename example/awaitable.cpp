#include "awaitable.h"
#include "coroutine.hpp"

#include <iostream>

kiedis::Task<void> get()
{
    for (int i = 0; i < 10; i++)
    {
        std::cout << "Before " << i << std::endl;
        auto j = co_await kiedis::Awaitable{};
        std::cout << "After " << i << std::endl;
    }
}

int main()
{
    for (auto i = 0; i < 10; i++)
    {
        auto t = get();
    }

    return 0;
}