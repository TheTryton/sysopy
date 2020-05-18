#include <sockets/ip/tcp/protocol.hpp>

TCP_NAMESPACE_BEGIN

tcp::tcp(const sockets::ip::address &address)
    : _address(address)
{

}

tcp::operator address() const
{
    return _address;
}

tcp v4() {
    return tcp(address_v4::any());
}

tcp v6() {
    return tcp(address_v6::any());
}

TCP_NAMESPACE_END