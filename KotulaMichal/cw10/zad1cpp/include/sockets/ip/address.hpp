#pragma once

#include <sockets/common.hpp>
#include <sockets/ip/defines.hpp>

IP_NAMESPACE_BEGIN

class address_v4 final {
private:
    in_addr _address;
private:
    address_v4(in_addr&& address);
public:
    address_v4() = delete;
    address_v4(const address_v4& other) = default;
    address_v4(address_v4&& other) noexcept = default;
public:
    address_v4& operator=(const address_v4& other) = default;
    address_v4& operator=(address_v4&& other) noexcept = default;
public:
    string to_string() const;
public:
    in_addr native_address() const;
public:
    static address_v4 any();
    static address_v4 from_string(string_view dotted_notation);
};

class address_v6 final {
private:
    in6_addr _address;
private:
    address_v6(in6_addr&& address);
public:
    address_v6() = delete;
    address_v6(const address_v6& other) = default;
    address_v6(address_v6&& other) noexcept = default;
public:
    address_v6& operator=(const address_v6& other) = default;
    address_v6& operator=(address_v6&& other) noexcept = default;
public:
    string to_string() const;
public:
    in6_addr native_address() const;
public:
    static address_v6 any();
    static address_v6 from_string(string_view hex_notation);
};

class address final {
private:
    variant<address_v4, address_v6> _address;
public:
    address() = delete;
    address(const address_v4& addressV4);
    address(address_v4&& addressV4);
    address(const address_v6& addressV6);
    address(address_v6&& addressV6);
    address(const address& other) = default;
    address(address&& other) noexcept = default;
public:
    address& operator=(const address& other) = default;
    address& operator=(address&& other) noexcept = default;
public:
    bool is_v4() const;
    bool is_v6() const;
    address_v4 to_v4() const;
    address_v6 to_v6() const;
    string to_string() const;
public:
    variant<in_addr, in6_addr> native_address() const;
public:
    static address from_string(string_view ipv4_or_ipv6_notation);
};

IP_NAMESPACE_END