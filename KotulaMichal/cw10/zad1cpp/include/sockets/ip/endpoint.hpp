#pragma once

#include <sockets/common.hpp>
#include <sockets/ip/address.hpp>

IP_NAMESPACE_BEGIN

template <class InternetProtocol>
class basic_endpoint
{
private:
    IP_NAMESPACE_FULL::address _address;
    unsigned short _port;
public:
    basic_endpoint(const InternetProtocol& protocol, unsigned short port)
        : _address(protocol)
        , _port(port)
    {}
    basic_endpoint(const address& addr, unsigned short port)
        : _address(addr)
        , _port(port)
    {}
    basic_endpoint(const basic_endpoint& other) = default;
    basic_endpoint(basic_endpoint&& other) noexcept= default;
public:
    basic_endpoint& operator=(const basic_endpoint& other) = default;
    basic_endpoint& operator=(basic_endpoint&& other) noexcept = default;
public:
    IP_NAMESPACE_FULL::address address() const
    {
        return _address;
    }
    unsigned short port() const
    {
        return _port;
    }
public:
    void set_address(const IP_NAMESPACE_FULL::address& addr)
    {
        _address = addr;
    }
    void set_port(unsigned short port)
    {
        _port = port;
    }
};

IP_NAMESPACE_END