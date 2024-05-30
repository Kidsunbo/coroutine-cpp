#ifndef COROUTINECPP_UTILS_H
#define COROUTINECPP_UTILS_H

#include <source_location>
#include <iostream>

namespace co_cpp
{
    // set the file discriptor non-blocking. return false if failed
    bool set_non_blocking(int sock);

    template <typename... T>
    struct log
    {
        log(T &&...args, std::source_location loc = std::source_location::current())
        {
            std::cout << "[LOG " << loc.file_name() << ":" << loc.line() << " " << loc.function_name() << "]";
            ((std::cout << args << " "), ...);
            std::cout << std::endl;
        }
    };

    template <typename... T>
    log(T &&...args) -> log<T...>;
} // namespace co_cpp

#endif