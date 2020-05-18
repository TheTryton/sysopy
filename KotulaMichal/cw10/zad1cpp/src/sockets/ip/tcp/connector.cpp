#include <sockets/ip/tcp/connector.hpp>

TCP_NAMESPACE_BEGIN

connector::connector(io_context &context)
        : _context(context)
{}

endpoint connector::connect(socket &peer, const endpoint &peer_endpoint, error_code &ec) {
    int sockfd = -1;
    if(peer_endpoint.address().is_v4())
    {
        sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    } else{
        sockfd = ::socket(AF_INET6, SOCK_STREAM, 0);
    }

    if(sockfd == -1)
    {
        ec = make_error_code(static_cast<socket_error>(errno));
        return peer_endpoint;
    }

    do {
        ec = connect_impl(sockfd, peer_endpoint);
    }while(ec == make_error_code(connect_error::retry));

    if(ec != error_code()) {
        peer.set_sockfd(sockfd);
    } else{
        close(sockfd);
    }

    return peer_endpoint;
}

error_code connector::connect_impl(int sockfd, const endpoint& peer_address) {
    variant<sockaddr_in, sockaddr_in6> server_address_union;

    if(peer_address.address().is_v4())
    {
        sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr = std::get<0>(peer_address.address().native_address());
        server_addr.sin_port = htons(peer_address.port());
        server_address_union = server_addr;
    }
    else
    {
        sockaddr_in6 server_addr;
        server_addr.sin6_family = AF_INET6;
        server_addr.sin6_addr = std::get<1>(peer_address.address().native_address());
        server_addr.sin6_port = htons(peer_address.port());
        server_address_union = server_addr;
    }

    const sockaddr* server_address = std::visit([](auto&& v){return reinterpret_cast<const sockaddr*>(&v);}, server_address_union);
    size_t server_address_size = std::visit([](auto&& v){return sizeof(v);}, server_address_union);

    if(::connect(sockfd, server_address, server_address_size) == -1)
    {
        return make_error_code(static_cast<connect_error>(errno));
    } else{
        return error_code();
    }
}

TCP_NAMESPACE_END

