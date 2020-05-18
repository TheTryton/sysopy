#pragma once

#include <sockets/ip/tcp/defines.hpp>
#include <sockets/ip/tcp/socket.hpp>

#include <sockets/ip/ip.hpp>

#include <sockets/sockets.hpp>

#include <sockets/errors/errors.hpp>

TCP_NAMESPACE_BEGIN

class connector final
{
private:
    io_context& _context;
public:
    connector(io_context& context);
private:
    error_code connect_impl(int sockfd, const endpoint& peer_address);
    template<class F>
    void async_connect_impl(socket& peer, shared_ptr<int> sockfd, const endpoint& peer_endpoint, F&& cch)
    {
        error_code ec = connect_impl(*sockfd, peer_endpoint);
        if(ec == make_error_code(connect_error::retry))
        {
            _context.post([this, &peer, sockfd = std::move(sockfd), &peer_endpoint, cch = std::move(cch)](){
                async_connect_impl(peer, std::move(sockfd), peer_endpoint, std::move(cch));
            });
        }
        else {
            peer.set_sockfd(*sockfd);
            *sockfd = -1;
            cch(peer_endpoint, ec);
        }
    }
public:
    endpoint connect(socket& peer, const endpoint& peer_endpoint, error_code& ec);
    template<class F>
    void async_connect(socket& peer, const endpoint& peer_endpoint, F&& connect_completion_handler)
    {
        _context.post([this, &peer, &peer_endpoint, cch = std::move(connect_completion_handler)](){
            shared_ptr<int> sockfd = shared_ptr<int>(new int(-1), [](int* v){
                if(*v != -1)
                {
                    ::close(*v);
                }
                delete v;
            });

            if(peer_endpoint.address().is_v4())
            {
                *sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
            } else{
                *sockfd = ::socket(AF_INET6, SOCK_STREAM, 0);
            }

            if(*sockfd == -1)
            {
                error_code ec = make_error_code(static_cast<socket_error>(errno));
                cch(peer_endpoint, ec);
            } else{
                async_connect_impl(peer, std::move(sockfd), peer_endpoint, std::move(cch));
            }
        });
    }
};

TCP_NAMESPACE_END