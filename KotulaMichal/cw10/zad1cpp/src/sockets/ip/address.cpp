#include <sockets/ip/address.hpp>

IP_NAMESPACE_BEGIN

address_v4::address_v4(in_addr &&address)
        : _address(std::move(address))
{
}

address_v4 address_v4::from_string(string_view dotted_notation) {
    in_addr addr{};

    if(inet_pton(AF_INET, dotted_notation.data(), &addr) == 0)
    {
        throw std::runtime_error("Invalid ip address string.");
    }

    return address_v4(std::move(addr));
}

address_v4 address_v4::any() {
    in_addr addr{};
    addr.s_addr = INADDR_ANY;
    return address_v4(std::move(addr));
}

string address_v4::to_string() const {
    char repr[INET_ADDRSTRLEN+1];
    repr[INET_ADDRSTRLEN]='\n';
    return inet_ntop(AF_INET,&_address,repr, INET_ADDRSTRLEN);
}

in_addr address_v4::native_address() const {
    return _address;
}

address_v6::address_v6(in6_addr &&address)
    : _address(std::move(address))
{

}

string address_v6::to_string() const {
    char repr[INET6_ADDRSTRLEN+1];
    repr[INET6_ADDRSTRLEN]='\n';
    return inet_ntop(AF_INET6,&_address,repr, INET6_ADDRSTRLEN);
}

address_v6 address_v6::any() {
    return address_v6(in6_addr(in6addr_any));
}

address_v6 address_v6::from_string(string_view hex_notation) {
    in6_addr addr{};

    if(inet_pton(AF_INET6, hex_notation.data(), &addr) == 0)
    {
        throw std::runtime_error("Invalid ip address string.");
    }

    return address_v6(std::move(addr));
}

in6_addr address_v6::native_address() const {
    return _address;
}

address::address(const address_v4 &addressV4)
    : _address(addressV4)
{

}

address::address(address_v4 &&addressV4)
    : _address(std::move(addressV4))
{

}

address::address(const address_v6 &addressV6)
    : _address(addressV6)
{

}

address::address(address_v6&& addressV6)
    : _address(std::move(addressV6))
{

}

bool address::is_v4() const {
    return _address.index() == 0;
}

bool address::is_v6() const {
    return _address.index() == 1;
}

address_v4 address::to_v4() const {
    if(_address.index() == 0)
    {
        return std::get<0>(_address);
    } else{
        throw std::runtime_error("Address does not contain IPv4 address.");
    }
}

address_v6 address::to_v6() const {
    if(_address.index() == 1)
    {
        return std::get<1>(_address);
    } else{
        throw std::runtime_error("Address does not contain IPv6 address.");
    }
}


string address::to_string() const {
    return std::visit([](auto&& v){return v.to_string();}, _address);
}

address address::from_string(string_view ipv4_or_ipv6_notation) {
    try {
        return address(address_v4::from_string(ipv4_or_ipv6_notation));
    }
    catch(...)
    {
        try {
            return address(address_v6::from_string(ipv4_or_ipv6_notation));
        }
        catch(...)
        {
            throw std::runtime_error("String does not contain IPv4 nor IPv6.");
        }
    }
}

variant<in_addr, in6_addr> address::native_address() const {
    return std::visit([](auto&& v) -> variant<in_addr, in6_addr> {return v.native_address();}, _address);
}

IP_NAMESPACE_END
