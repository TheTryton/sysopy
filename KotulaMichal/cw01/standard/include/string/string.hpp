#pragma once

#include <common.hpp>

STD_LIB_NAMESPACE_BEGIN

class string
{
private:
    char* _buff = nullptr;
    size_t _size = 0;
private:
    void destruct();
public:
    string() = default;
    string(const char* t);
    string(const char* t, size_t n);
    string(size_t s);
    string(size_t s, char c);
    string(const string& other);
    string(string&& other) noexcept;
public:
    ~string();
public:
    string& operator=(const char* t);
    string& operator=(const string& other);
    string& operator=(string&& other) noexcept;
public:
    static constexpr size_t npos = std::numeric_limits<size_t>::max();
public:
    void clear();
    void resize(size_t n);
    const char* c_str() const;
    char* c_str();
public:
    template<class F>
    size_t find(size_t from, F&& f) const;
    vector<string> split(char delim = ' ') const;
    string substr(size_t from, size_t size = string::npos) const;
public:
    size_t size() const;

    char* begin();
    const char* begin() const;

    char* end();
    const char* end() const;

    char* data();
    const char* data() const;

    char& operator[](size_t i);
    const char& operator[](size_t i) const;

    char& front();
    const char& front() const;
    char& back();
    const char& back() const;
public:
    bool operator==(const string& other) const;
    bool operator!=(const string& other) const;
};

template<class F>
size_t string::find(size_t from, F &&f) const {
    auto b = _buff + from;
    auto e = _buff + _size;

    while (b != e) {
        if (f(*b)) {
            return b - _buff;
        }
        b++;
    }

    return string::npos;
}

STD_LIB_NAMESPACE_END