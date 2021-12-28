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
        auto ret = recv(fd, buf.data(), buf.size(), 0);
        if (ret == 0)
        {
            connection_close = true;
            return false;
        }
        while (ret > 0 && (errno != EAGAIN && errno != EWOULDBLOCK))
        {
            result.append(std::string{buf.data(), static_cast<unsigned long>(ret)});
            need_suspend = false;
            ret = recv(fd, buf.data(), buf.size(), 0);
        }
        handle = h; // public the coroutine handler here so that the resume triggered by the above code in another thread will not call await_resume and the destructor.
        return need_suspend;
    }

    std::tuple<std::string, bool> ReadFuture::await_resume()
    {
        if (connection_close)
        {
            return {"", false};
        }
        if (need_suspend)
        {
            auto ret = recv(fd, buf.data(), buf.size(), 0);
            {
                connection_close = true;
            }
            while (ret > 0 && (errno != EAGAIN || errno != EWOULDBLOCK))
            {
                result.append(std::string{buf.data(), static_cast<unsigned long>(ret)});
                ret = recv(fd, buf.data(), buf.size(), 0);
            }
        }
        return {std::move(result), true};
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
        auto ret = send(fd, content.data(), content.size(), 0);
        if (ret == 0)
        {
            connection_close = true;
            return false;
        }
        while (total_write < content.size() && ret > 0 && (errno != EAGAIN && errno != EWOULDBLOCK))
        {
            total_write += ret;
            if (total_write >= content.size())
            {
                need_suspend = false;
                break;
            }
            ret = send(fd, content.data() + total_write, content.size() - total_write, 0);
        }
        handle = h; // public the coroutine handler here so that the resume triggered by the above code in another thread will not call await_resume and the destructor.
        return need_suspend;
    }

    std::tuple<unsigned long, bool> WriteFuture::await_resume()
    {
        if (connection_close)
        {
            return {0, false};
        }
        if (need_suspend)
        {
            auto ret = send(fd, content.data() + total_write, content.size() - total_write, 0);
            if (ret == 0)
            {
                connection_close = true;
                return {0, false};
            }
            while (total_write < content.size() && ret > 0 && (errno != EAGAIN && errno != EWOULDBLOCK))
            {
                total_write += ret;
                if (total_write >= content.size())
                {
                    break;
                }
                ret = send(fd, content.data() + total_write, content.size() - total_write, 0);
            }
        }
        return {total_write, true};
    }

    bool WriteFuture::valid()
    {
        return !connection_close;
    }

    AcceptFuture::AcceptFuture(int fd, std::coroutine_handle<>& h) : fd(fd), handle(h) {}

    bool AcceptFuture::await_ready()
    {
        return !result.empty();
    }

    bool AcceptFuture::await_suspend(std::coroutine_handle<> h)
    {
        sockaddr_in address{};
        socklen_t len;
        auto sock = accept(fd, reinterpret_cast<sockaddr *>(&address), &len);
        if (sock == 0)
        {
            connection_close = true;
            return false;
        }
        while (sock > 0 && (errno != EAGAIN && errno != EWOULDBLOCK))
        {
            result.push(sock);
            sock = accept(fd, reinterpret_cast<sockaddr *>(&address), &len);
        }
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
        socklen_t len;
        auto sock = accept(fd, reinterpret_cast<sockaddr *>(&address), &len);
        if (sock <= 0 && result.empty())
        {
            return {0, false};
        }
        while (sock > 0 && (errno != EAGAIN && errno != EWOULDBLOCK))
        {
            result.push(sock);
            sock = accept(fd, reinterpret_cast<sockaddr *>(&address), &len);
        }
        auto ret = result.front();
        result.pop();
        return {ret, true};
    }

    bool AcceptFuture::valid()
    {
        return !connection_close;
    }

} // namespace kiedis
