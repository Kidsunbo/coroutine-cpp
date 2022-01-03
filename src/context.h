#ifndef KIEDIS_CONTEXT_H
#define KIEDIS_CONTEXT_H

/**
 * This file define a class wrapper for epoll to handle the context and file descriptor for the whole library.
 **/

#include <memory>
#include <coroutine>
#include <unordered_map>
#include <functional>
#include "utils.h"

#include "coroutine.hpp"

namespace kiedis
{


    enum class NextState{
        Nothing = 0,
        Error = 1,
        In    = 2,
        Out   = 3,
    };

    struct EventData{
        NextState state = NextState::Nothing;
        std::coroutine_handle<> h;
    };

    class IOContext
    {
        friend struct FinalAwaiter;
        friend void remove_from_context(IOContext &ctx, std::coroutine_handle<> h);
        friend void co_spawn(IOContext &ctx, Task<void> &&t);
        friend void add_to_context(IOContext &ctx, Task<void> &&t);

        int epoll_fd;
        std::unordered_map<std::coroutine_handle<>, std::unique_ptr<Task<void>>> tasks;

    public:
        IOContext();
        IOContext(IOContext &&ctx) = delete;
        ~IOContext() noexcept;

        void run();
        int get_epoll_fd();

    };

} // namespace kiedis

#endif