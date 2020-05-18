#include <sockets/ip/tcp/socket.hpp>

TCP_NAMESPACE_BEGIN

socket::socket(io_context &context, const endpoint &endpoint)
        : _context(context)
{
    auto family = endpoint.address().is_v4() ?
                  AF_INET :
                  (endpoint.address().is_v6() ? AF_INET6 : -1);

    _sockfd = ::socket(
            family,
            SOCK_STREAM | SOCK_NONBLOCK,
            0);

    if(_sockfd == -1)
    {
        throw std::runtime_error(make_error_code(static_cast<socket_error>(errno)).message());
    }

    variant<sockaddr_in, sockaddr_in6> server_address_union;

    if(family == AF_INET)
    {
        sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr = std::get<0>(endpoint.address().native_address());
        server_addr.sin_port = htons(endpoint.port());
        server_address_union = server_addr;
    }
    else
    {
        sockaddr_in6 server_addr;
        server_addr.sin6_family = AF_INET6;
        server_addr.sin6_addr = std::get<1>(endpoint.address().native_address());
        server_addr.sin6_port = htons(endpoint.port());
        server_address_union = server_addr;
    }

    const sockaddr* server_address = std::visit([](auto&& v){return reinterpret_cast<const sockaddr*>(&v);}, server_address_union);
    size_t server_address_size = std::visit([](auto&& v){return sizeof(v);}, server_address_union);

    if(::bind(_sockfd, server_address, server_address_size) == -1)
    {
        ::close(_sockfd);
        _sockfd = -1;
        throw std::runtime_error(make_error_code(static_cast<bind_error>(errno)).message());
    }

    if(::listen(_sockfd, 16) == -1)
    {
        ::close(_sockfd);
        _sockfd = -1;
        throw std::runtime_error(make_error_code(static_cast<listen_error>(errno)).message());
    }
}


void socket::set_sockfd(int sockfd) {
    if(_sockfd!=-1)
    {
        ::close(_sockfd);
    }
    _sockfd = sockfd;
}

int socket::sockfd() const {
    return _sockfd;
}

socket::socket(io_context &context)
        : _context(context)
{

}

socket::socket(socket &&other) noexcept
    : _context(other._context)
    , _sockfd(other._sockfd)
{
    other._sockfd = -1;
}

socket& socket::operator=(socket &&other) noexcept {
    _sockfd = other._sockfd;
    other._sockfd = -1;
    return *this;
}

socket::~socket() {
    if(_sockfd!=-1)
    {
        ::shutdown(_sockfd, SHUT_RD);
        ::close(_sockfd);
    }
}

size_t socket::read_some(byte *buffer, size_t size, error_code &ec) {
    return recv(_sockfd, buffer, size, MSG_DONTWAIT);
}

size_t socket::read(byte *buffer, size_t size, error_code &ec) {
    size_t read_bytes = 0;
    while(size != 0)
    {
        auto read_bytes_count = read_some(buffer, size, ec);
        read_bytes += read_bytes_count;
        if(ec != error_code())
        {
            return read_bytes;
        }
        buffer += read_bytes_count;
        size -= read_bytes_count;
    }
    return read_bytes;
}

size_t socket::write(const byte *buffer, size_t size, error_code &ec) {
    size_t written_bytes = 0;
    while(size != 0)
    {
        auto written_bytes_count = write_some(buffer, size, ec);
        written_bytes += written_bytes_count;
        if(ec != error_code())
        {
            return written_bytes;
        }
        buffer += written_bytes;
        size -= written_bytes;
    }
    return written_bytes;
}

size_t socket::write_some(const byte *buffer, size_t size, error_code &ec) {
    return send(_sockfd, buffer, size, MSG_DONTWAIT);
}

address socket::socket_address() const {
    sockaddr_in saddr;
    socklen_t saddrlen = sizeof(saddr);
    getsockname(_sockfd, reinterpret_cast<sockaddr*>(&saddr), &saddrlen);

    return address_v4::from_string(inet_ntoa(saddr.sin_addr));
}

TCP_NAMESPACE_END