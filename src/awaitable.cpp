#include "awaitable.h"
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/in.h>

namespace kiedis
{

    ReadFuture::ReadFuture(int fd, std::coroutine_handle<> &h) : fd(fd), handle(h) {}

    // For now, there is no optimization here, always call await_suspend anyway.
    bool ReadFuture::await_ready()
    {
        return false;
    }

    bool ReadFuture::await_suspend(std::coroutine_handle<> h)
    {
        handle = h; // public the coroutine handler here so that the resume triggered by the above code in another thread will not call await_resume and the destructor.
        return true;
    }

    std::tuple<std::string, bool> ReadFuture::await_resume()
    {
        if (connection_close)
        {
            return {"", false};
        }
        std::array<char, 1024> buf;
        auto ret = recv(fd, buf.data(), buf.size(), 0);
        {
            connection_close = true;
        }
        if (ret <= 0)
        {
            return {"", false};
        };
        return {std::string{buf.data(), static_cast<unsigned long>(ret)}, true};
    }

    bool ReadFuture::valid()
    {
        return !connection_close;
    }

    WriteFuture::WriteFuture(int fd, std::coroutine_handle<> &h, std::string content) : fd(fd), content(std::move(content)), handle(h) {}

    bool WriteFuture::await_ready()
    {
        return false;
    }

    bool WriteFuture::await_suspend(std::coroutine_handle<> h)
    {
        handle = h; // public the coroutine handler here so that the resume triggered by the above code in another thread will not call await_resume and the destructor.
        return true;
    }

    std::tuple<unsigned long, bool> WriteFuture::await_resume()
    {
        if (connection_close)
        {
            return {0, false};
        }
        auto ret = send(fd, content.data(), content.size(), 0);
        if (ret == 0)
        {
            connection_close = true;
            return {0, false};
        }

        return {ret, true};
    }

    bool WriteFuture::valid()
    {
        return !connection_close;
    }

    AcceptFuture::AcceptFuture(int fd, std::coroutine_handle<> &h) : fd(fd), handle(h) {}

    bool AcceptFuture::await_ready()
    {
        return false;
    }

    bool AcceptFuture::await_suspend(std::coroutine_handle<> h)
    {
        handle = h; // public the coroutine handler here so that the resume triggered by the above code in another thread will not call await_resume and the destructor.
        return true;
    }

    std::tuple<int, bool> AcceptFuture::await_resume()
    {

        if (connection_close)
        {
            return {0, false};
        }
        sockaddr_in address{};
        socklen_t len = 0;
        auto sock = accept(fd, reinterpret_cast<sockaddr*>(&address), &len);
        if (sock <= 0)
        {
            return {0, false};
        }
        return {sock, true};
    }

    bool AcceptFuture::valid()
    {
        return !connection_close;
    }

} // namespace kiedis
