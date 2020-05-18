#pragma once

#include <sockets/ip/tcp/defines.hpp>
#include <sockets/ip/address.hpp>

TCP_NAMESPACE_BEGIN

struct tcp
{
private:
    address _address;
public:
    tcp(const address& address);
public:
    operator address() const;
};

tcp v4();
tcp v6();

TCP_NAMESPACE_END