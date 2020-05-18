#pragma once

#include <sockets/ip/tcp/defines.hpp>
#include <sockets/ip/tcp/socket.hpp>

#include <sockets/ip/ip.hpp>

#include <sockets/sockets.hpp>

#include <sockets/errors/errors.hpp>

TCP_NAMESPACE_BEGIN

class acceptor final
{
private:
    socket _socket;
    io_context& _context;
public:
    acceptor(io_context& context, const endpoint& endpoint);
private:
    error_code accept_impl(socket& peer);
public:
    void accept(socket& peer, error_code& ec);
    template<class F>
    void async_accept(socket& peer, F&& accept_completion_handler)
    {
        _context.post([this, &peer, ach = std::move(accept_completion_handler)](){
            auto ec = accept_impl(peer);
            if(ec == make_error_code(accept_error::no_available_client))
            {
                async_accept(peer, std::move(ach));
            } else{
                ach(ec);
            }
        });
    }
};

TCP_NAMESPACE_END