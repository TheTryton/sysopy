#include <sockets/ip/tcp/acceptor.hpp>

TCP_NAMESPACE_BEGIN

acceptor::acceptor(io_context &context, const endpoint &endpoint)
        : _context(context)
        , _socket(context, endpoint)
{
}

error_code acceptor::accept_impl(socket &peer) {
    sockaddr_in client_addr;
    socklen_t socklen = sizeof(client_addr);
    peer.set_sockfd(accept4(
            _socket._sockfd,
            reinterpret_cast<struct sockaddr *>(&client_addr),
            &socklen,
            SOCK_NONBLOCK
    ));

    if(peer.sockfd() == -1)
    {
        return make_error_code(static_cast<accept_error>(errno));
    }

    return error_code();
}

void acceptor::accept(socket &peer, error_code &ec) {
    do {
        ec = accept_impl(peer);
    }
    while(ec == make_error_code(accept_error::no_available_client));
}

TCP_NAMESPACE_END