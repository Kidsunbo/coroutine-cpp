#include <coroutine.hpp>
#include <iostream>

kiedis::Task<int> get(){
    for(int i=0;i<10;i++){
        co_await std::suspend_always{};
        std::cout<<"inner "<<i<<std::endl;
    }
    co_return 100;
}


int main(){
    auto g = get();
    for(int i=0;i<101;i++){
        std::cout<<std::boolalpha<<g.next()<<std::endl;
    }
    std::cout<<g.value()<<std::endl;
    return 0;
}