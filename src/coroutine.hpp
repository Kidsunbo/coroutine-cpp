#ifndef KIEDIS_COROUTINE_HPP
#define KIEDIS_COROUTINE_HPP

#include <coroutine>
#include <utility>
#include <iostream>
#include "utils.h"


namespace this_core
{
    struct excutor_t
    {
    };

constexpr excutor_t excutor;
} // namespace this_core

namespace kiedis
{

    class IOContext;
    template<typename T>
    class Task;

    void remove_from_context(IOContext &ctx, std::coroutine_handle<> h);
    void add_to_context(IOContext &ctx, Task<void>&& t);
    void co_spawn(IOContext &ctx, Task<void> &&t);

    struct FinalAwaiter
    {
        std::coroutine_handle<> h;
        IOContext *ctx;
        FinalAwaiter(IOContext *ctx) : ctx(ctx) {}
        bool await_ready() noexcept
        {
            return false;
        }

        void await_suspend(std::coroutine_handle<> h) noexcept
        {
            remove_from_context(*ctx, h);
        }

        void await_resume() noexcept {}
    };

    template <typename Task>
    struct Promise
    {
        IOContext *ctx = nullptr;

        Task get_return_object()
        {
            return Task{std::coroutine_handle<Promise>::from_promise(*this)};
        }
        auto initial_suspend()
        {
            return std::suspend_always{};
        }
        auto final_suspend() noexcept
        {
            return FinalAwaiter{ctx};
        }
        void return_void()
        {
        }
        void unhandled_exception()
        {
        }

        auto await_transform(this_core::excutor_t)
        {
            struct awaiter
            {
                IOContext &ctx;
                explicit awaiter(IOContext &ctx) noexcept : ctx(ctx) {}
                bool await_ready() noexcept { return true; }
                void await_suspend(std::coroutine_handle<>) noexcept {}
                IOContext &await_resume() noexcept { return ctx; }
            };
            return awaiter{*ctx};
        }

        template<typename U>
        decltype(auto) await_transform(U&& u){
            return std::forward<U>(u);
        }

        void set_context(IOContext &ctx)
        {
            this->ctx = &ctx;
        }

        ~Promise(){
            log("【KIE】");
        }
    };

    template <typename T>
    struct Task
    {
        using promise_type = Promise<Task>;

        std::coroutine_handle<promise_type> co_handle;

        Task(std::coroutine_handle<promise_type> handle) : co_handle(handle) {}
        Task(Task &&t) : co_handle(std::exchange(t.co_handle, nullptr)) {}

        ~Task() noexcept
        {
            if (co_handle)
            {
                co_handle.destroy();
            }
        }
        bool next()
        {
            if (!co_handle.done())
            {
                co_handle.resume();
                return true;
            }
            return false;
        }

        T value()
        {
            return co_handle.promise().value;
        }
    };

    template <>
    struct Task<void>
    {
        using promise_type = Promise<Task>;

        std::coroutine_handle<promise_type> co_handle;

        Task(std::coroutine_handle<promise_type> handle) : co_handle(handle) {}
        Task(Task &&t) : co_handle(std::exchange(t.co_handle, nullptr)) {}

        Task &operator=(Task &&t)
        {
            this->co_handle = t.co_handle;
            t.co_handle = nullptr;
            return *this;
        }

        ~Task() noexcept
        {
            if (co_handle)
            {
                co_handle.destroy();
            }
        }
        bool next()
        {
            if (!co_handle.done())
            {
                co_handle.resume();
                return true;
            }
            return false;
        }
    };

} // namespace kiedis

namespace std
{
    template <class _Tp>
    struct hash<coroutine_handle<_Tp>>
    {
        size_t operator()(const coroutine_handle<_Tp> &__v) const noexcept
        {
            return hash<void *>()(__v.address());
        }
    };
} // namespace std

#endif