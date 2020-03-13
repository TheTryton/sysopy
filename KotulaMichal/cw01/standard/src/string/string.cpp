#include <string/string.hpp>
#include <vector/vector.hpp>

STD_LIB_NAMESPACE_BEGIN

void string::destruct()
{
    if(_buff)
    {
        freen(_buff, _size);
    }
}

string::string(const char *t) : _buff(strlen(t) > 0 ? calloc<char>(strlen(t)+1, sizeof(char)) : nullptr), _size(strlen(t))
{
    if(_size > 0)
    {
        strcpy(_buff, t);
    }
}

string::string(const char *t, size_t n) : _buff(n > 0 ? calloc<char>(n+1, sizeof(char)) : nullptr), _size(n)
{
    if(n > 0)
    {
        strncpy(_buff, t, n);
    }
}

string::string(size_t s) : _buff(s > 0 ? calloc<char>(s+1, sizeof(char)) : nullptr), _size(s)
{
    if(s > 0)
    {
        _buff[_size] = '\0';
    }
}

string::string(size_t s, char c) : _buff(s > 0 ? calloc<char>(s+1, sizeof(char)) : nullptr), _size(s)
{
    if(s > 0) {
        std::fill(_buff, _buff + _size, c);
        _buff[_size] = '\0';
    }
}

string::string(const string &other) : _buff(other._size > 0 ? calloc<char>(other._size+1, sizeof(char)) : nullptr), _size(other._size)
{
    std::copy(other._buff, other._buff +  other._size, _buff);
}

string::string(string &&other) noexcept : _buff(other._buff), _size(other._size)
{
    other._buff = nullptr;
    other._size = 0;
}

string::~string() {
    destruct();
}

string &string::operator=(const char *t) {
    destruct();
    if(strlen(t) > 0)
    {
        _buff = calloc<char>(strlen(t)+1, sizeof(char));
        _size = strlen(t);
        strcpy(_buff, t);
    }
    else
    {
        _buff = nullptr;
        _size = 0;
    }
    return *this;
}

string &string::operator=(const string &other) {
    destruct();
    if(other._size > 0)
    {
        _buff = calloc<char>(other._size+1, sizeof(char));
        _size = other._size;
        std::copy(other._buff, other._buff +  other._size, _buff);
    }
    else
    {
        _buff = nullptr;
        _size = 0;
    }
    return *this;
}

string &string::operator=(string &&other) noexcept {
    destruct();
    _buff = other._buff;
    _size = other._size;
    other._buff = nullptr;
    other._size = 0;
    return *this;
}

string string::substr(size_t from, size_t size) const {
    return string(_buff + from, std::min(_size, from + size) - from);
}

bool string::operator==(const string &other) const {
    if(other._size != _size)
    {
        return false;
    }
    return std::equal(_buff, _buff + _size, other._buff, [](char a, char b){return a==b;});
}

bool string::operator!=(const string &other) const {
    if(other._size != _size)
    {
        return true;
    }
    return !std::equal(_buff, _buff + _size, other._buff, [](char a, char b){return a==b;});
}

void string::clear() {
    destruct();
    _buff = nullptr;
    _size = 0;
}

const char *string::data() const {
    return _buff;
}

size_t string::size() const {
    return _size;
}

char *string::begin() {
    return _buff;
}

const char *string::begin() const {
    return _buff;
}

char *string::end() {
    return _buff + _size;
}

const char *string::end() const {
    return _buff + _size;
}

char *string::data() {
    return _buff;
}

char &string::operator[](size_t i) {
    return _buff[i];
}

const char &string::operator[](size_t i) const {
    return _buff[i];
}

const char *string::c_str() const {
    return _buff;
}

char *string::c_str() {
    return _buff;
}

char &string::front() {
    return _buff[0];
}

const char &string::front() const {
    return _buff[0];
}

char &string::back() {
    return _buff[_size-1];
}

const char &string::back() const {
    return _buff[_size-1];
}

void string::resize(size_t n) {
    auto new_data = calloc<char>(n + 1, sizeof(char));
    std::copy(_buff, _buff + std::min(n, _size), new_data);
    new_data[n+1]='\0';
    destruct();
    _buff = new_data;
    _size = n;
}

vector<string> string::split(char delim) const {
    vector<string> splitted;
    auto is_delim = [=](char c) {return c == delim;};
    size_t previous = 0;
    size_t current = 0;
    current = find(current, is_delim);
    while(current != npos)
    {
        splitted.push_back(substr(previous, current-previous));
        previous = current+1;
        current = find(previous, is_delim);
    }
    auto last_s = substr(previous, current-previous);
    splitted.push_back(last_s);
    return splitted;
}

STD_LIB_NAMESPACE_END