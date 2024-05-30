# CoroutineCPP

CoroutineCPP is a wrapper for epoll and socket which is implemented with C++20. It supports the syntax of coroutine and can be easily used out of box.

## Feature
- coroutine supported

## Usage

If you use CMake for your project. All you need is adding some code to your `CMakeLists.txt` and add `coroutinecpp` as a dependency.

```cmake
    add_subdirectory(coroutinecpp)

    target_link_library(<your_project> PUBLIC coroutinecpp)
```

## Example
You can write a echo server like this.

```c++
co_cpp::Task<void> echo(co_cpp::Socket sock)
{
    auto &ctx = co_await this_core::excutor;
    while (true)
    {
        auto [content, ok_read] = co_await sock.read();
        if (!ok_read)
        {
            co_cpp::log("return code:", ok_read);
            co_return;
        }
        auto [len, ok_write] = co_await sock.write(content);
        if (!ok_write)
        {
            co_cpp::log("return code:", ok_write);
            co_return;
        }
    }
}

co_cpp::Task<void> listen(co_cpp::Socket sock)
{
    auto &ctx = co_await this_core::excutor;
    while (true)
    {
        auto [sock_fd, ok] = co_await sock.accept();
        if (!ok)
        {
            co_return;
        }
        co_cpp::co_spawn(ctx, echo(co_cpp::Socket{ctx,sock_fd}));
    }
}

int main()
{
    co_cpp::IOContext ctx;
    co_cpp::Socket socket(ctx);
    socket.bind(8080);
    co_cpp::co_spawn(ctx, listen(std::move(socket)));
    ctx.run();

    return 0;
}

```
Getting the context in your coroutine frame is easy, just use `co_await this_core::excutor`, the reference of context is returned immediately.

`co_cpp::Task<T>` holds the `promise_type` which is needed in coroutine. Forget the `promise_type`, the lifetime is handled well by `Task` and `IOContext`.

