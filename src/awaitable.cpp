#include "awaitable.h"
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <sys/epoll.h>

namespace co_cpp
{

    ReadFuture::ReadFuture(int fd, int epoll_fd) : fd(fd), epoll_fd(epoll_fd) {}

    bool ReadFuture::await_ready()
    {
        return false;
    }

    bool ReadFuture::await_suspend(std::coroutine_handle<> h)
    {

        epoll_event ev{};
        ev.events = EPOLLIN | EPOLLET;
        data.h = h;
        ev.data.ptr = &data;
        auto ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);
        log("", ret);
        if (ret < 0)
        {
            data.state = NextState::Error;
            return false;
        }

        return true;
    }

    std::tuple<std::string, bool> ReadFuture::await_resume()
    {
        if (data.state != NextState::In)
        {
            return {"", false};
        }

        std::array<char, 1024> buf;
        auto ret = recv(fd, buf.data(), buf.size(), 0);
        if (ret <= 0)
        {
            log(buf.size());
            return {"", false};
        };
        return {std::string{buf.data(), static_cast<unsigned long>(ret)}, true};
    }

    ReadFuture::~ReadFuture() noexcept
    {
        auto ret_epoll = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
        if (ret_epoll < 0)
        {
            log("return code is not zero");
        }
    }

    WriteFuture::WriteFuture(int fd, std::string content, int epoll_fd) : fd(fd), content(std::move(content)), epoll_fd(epoll_fd) {}

    bool WriteFuture::await_ready()
    {
        return false;
    }

    bool WriteFuture::await_suspend(std::coroutine_handle<> h)
    {
        log(static_cast<int>(data.state));

        epoll_event ev{};
        ev.events = EPOLLOUT | EPOLLET;
        data.h = h;
        ev.data.ptr = &data;
        auto ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);
        if (ret < 0)
        {
            log("", ret);
            data.state = NextState::Error;
            return false;
        }
        log("here");
        return true;
    }

    std::tuple<unsigned long, bool> WriteFuture::await_resume()
    {
        log(static_cast<int>(data.state), content);

        if (data.state != NextState::Out)
        {
            return {0, false};
        }
        auto ret = send(fd, content.data(), content.size(), 0);
        if (ret <= 0)
        {
            log(content, ret);
            return {0, false};
        }
        return {ret, true};
    }

    WriteFuture::~WriteFuture() noexcept
    {
        auto ret_epoll = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
        if (ret_epoll < 0)
        {
            log("return code is not zero");
        }
    }

    AcceptFuture::AcceptFuture(int fd, int epoll_fd) : fd(fd), epoll_fd(epoll_fd)
    {
        std::cout << "AcceptFuture" << std::endl;
    }

    bool AcceptFuture::await_ready()
    {
        return false;
    }

    bool AcceptFuture::await_suspend(std::coroutine_handle<> h)
    {
        log(h.address());

        epoll_event ev{};
        ev.events = EPOLLIN;
        data.h = h;
        ev.data.ptr = &data;
        auto ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);

        return true;
    }

    std::tuple<int, bool> AcceptFuture::await_resume()
    {
        log(static_cast<int>(data.state));
        if (data.state != NextState::In)
        {
            return {0, false};
        }
        sockaddr_in address{};
        socklen_t len = 0;
        auto sock = accept(fd, reinterpret_cast<sockaddr *>(&address), &len);
        if (sock <= 0)
        {
            return {0, false};
        }
        log("hello");
        return {sock, true};
    }

    AcceptFuture::~AcceptFuture()
    {
        auto ret_epoll = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
        if (ret_epoll < 0)
        {
            log("return code is not zero");
        }
    }

} // namespace co_cpp
