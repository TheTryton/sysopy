#pragma once

#include <sockets/ip/tcp/defines.hpp>
#include <sockets/ip/tcp/endpoint.hpp>

#include <sockets/ip/ip.hpp>

#include <sockets/sockets.hpp>

#include <sockets/errors/errors.hpp>

TCP_NAMESPACE_BEGIN

class socket final
{
    friend class acceptor;
    friend class connector;
private:
    int _sockfd = -1;
    io_context& _context;
private:
    socket(io_context& context, const endpoint& endpoint);
    void set_sockfd(int sockfd);
    int sockfd() const;
public:
    explicit socket(io_context& context);

    socket() = delete;
    socket(const socket& other) = delete;
    socket(socket&& other) noexcept;
public:
    socket& operator=(const socket& other) = delete;
    socket& operator=(socket&& other) noexcept;
public:
    ~socket();
public:
    address socket_address() const;
private:
    template<class F>
    void async_read_impl(byte* buffer, size_t size, size_t read_so_far, F&& read_completion_handler)
    {
        if(size == 0)
        {
            error_code ec;
            read_completion_handler(read_so_far, ec);
        }
        else
        {
            _context.post([this, buffer, size, read_so_far, rch = std::forward<F>(read_completion_handler)](){
                error_code ec;
                auto bytes_read = read_some(buffer, size, ec);
                size_t rsf = read_so_far;
                rsf += bytes_read;
                if(ec != error_code())
                {
                    rch(rsf, ec);
                } else{
                    async_read_impl(buffer+bytes_read, size - bytes_read, rsf, std::move(rch));
                }
            });
        }
    }

    template<class F>
    void async_write_impl(const byte* buffer, size_t size, size_t written_so_far, F&& write_completion_handler)
    {
        if(size == 0)
        {
            error_code ec;
            write_completion_handler(written_so_far, ec);
        }
        else {
            _context.post([this, buffer, size, written_so_far, wch = std::forward<F>(write_completion_handler)]() {
                error_code ec;
                auto bytes_written = write_some(buffer, size, ec);
                size_t wsf = written_so_far;
                wsf += bytes_written;
                if (ec != error_code()) {
                    wch(wsf, ec);
                } else {
                    async_read_impl(buffer + bytes_written, size - bytes_written, wsf, std::move(wch));
                }
            });
        }
    }
public:
    size_t read_some(byte* buffer, size_t size, error_code& ec);
    size_t read(byte* buffer, size_t size, error_code& ec);
    template<class F>
    void async_read_some(byte* buffer, size_t size, F&& read_some_completion_handler)
    {
        _context.post([this, buffer, size, rsch = std::forward<F>(read_some_completion_handler)](){
            error_code ec;
            auto bytes_read = read_some(buffer, size, ec);
            rsch(bytes_read, ec);
        });
    }
    template<class F>
    void async_read(byte* buffer, size_t size, F&& read_completion_handler)
    {
        async_read_impl(buffer, size, 0, std::forward<F>(read_completion_handler));
    }
public:
    size_t write_some(const byte* buffer, size_t size, error_code& ec);
    size_t write(const byte* buffer, size_t size, error_code& ec);
    template<class F>
    void async_write_some(const byte* buffer, size_t size, F&& write_some_completion_handler)
    {
        _context.post([this, buffer, size, wsch = std::forward<F>(write_some_completion_handler)](){
            error_code ec;
            auto bytes_written = write_some(buffer, size, ec);
            wsch(bytes_written, ec);
        });
    }
    template<class F>
    void async_write(const byte* buffer, size_t size, F&& write_completion_handler)
    {
        async_write_impl(buffer, size, 0, std::forward<F>(write_completion_handler));
    }
};

TCP_NAMESPACE_END