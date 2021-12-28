#ifndef KIEDIS_COROUTINE_HPP
#define KIEDIS_COROUTINE_HPP

#include <coroutine>
#include <utility>

#include <iostream>

namespace kiedis
{
    template <typename T>
    struct Task
    {
        struct promise_type
        {
            T value;

            Task get_return_object()
            {
                return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
            }
            auto initial_suspend()
            {
                return std::suspend_never{};
            }
            auto final_suspend() noexcept
            {
                return std::suspend_always{};
            }
            void return_value(T t)
            {
                value = std::move(t);
            }
            void unhandled_exception()
            {
            }
        };

        std::coroutine_handle<promise_type> co_handle;

        Task(std::coroutine_handle<promise_type> handle) : co_handle(handle) {}
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
        struct promise_type
        {
            std::coroutine_handle<> co_handle;

            Task get_return_object()
            {
                return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
            }
            auto initial_suspend()
            {
                return std::suspend_never{};
            }
            auto final_suspend() noexcept
            {
                return std::suspend_always{};
            }
            void return_void()
            {
            }
            void unhandled_exception()
            {
            }
        };

        std::coroutine_handle<promise_type> co_handle;

        Task(std::coroutine_handle<promise_type> handle) : co_handle(handle) {}
        Task(Task&& t):co_handle(t.co_handle){
            t.co_handle = nullptr;
        }

        Task& operator=(Task&& t){
            this->co_handle = t.co_handle;
            t.co_handle = nullptr;
            return *this;
        }

        ~Task() noexcept
        {
            std::cout << "~Task" << std::endl;
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

        bool await_ready()
        {
            return false;
        }

        void await_suspend(std::coroutine_handle<> h)
        {

        }

        void await_resume() noexcept
        {
        }
    };

} // namespace kiedis

#endif