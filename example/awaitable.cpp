#include <coroutine>
#include <thread>
#include "coroutine.hpp"
#include <iostream>
#include <functional>

kiedis::Task<void> get(){
    co_return;
}

int main()
{
    std::cout<<std::hash<std::coroutine_handle<void>>{}(std::noop_coroutine())<<std::endl;
    return 0;
}