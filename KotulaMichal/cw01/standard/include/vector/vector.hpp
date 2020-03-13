#pragma once

#include <common.hpp>

STD_LIB_NAMESPACE_BEGIN

template<class T>
class vector
{
private:
    T* _data = nullptr;
    size_t _size = 0;
private:
    void destruct()
    {
        if(_data)
        {
            freen(_data, _size);
        }
    }
public:
    vector() = default;
    vector(size_t n, const T& v = T()) : _data(calloc<T>(n, sizeof(T))), _size(n)
    {
        std::fill(_data, _data + _size, v);
    }
    vector(std::initializer_list<T> il) : _data(calloc<T>(il.size(), sizeof(T))), _size(il.size()) {
        std::copy(il.begin(), il.end(), _data);
    }
    vector(const vector& other) : _data(calloc<T>(other._size, sizeof(T))), _size(other._size)
    {
        std::copy(other._data, other._data + _size, _data);
    }
    vector(vector&& other) : _data(other._data), _size(other._size)
    {
        other._data = nullptr;
        other._size = 0;
    }
public:
    ~vector()
    {
        destruct();
    }
public:
    vector& operator=(const vector& other)
    {
        destruct();
        _data = calloc<T>(other._size, sizeof(T));
        _size = other._size;
        std::copy(other._data, other._data + _size, _data);
        return *this;
    }
    vector& operator=(vector&& other) {
        destruct();
        _data = other._data;
        _size = other._size;
        other._data = nullptr;
        other._size = 0;
        return *this;
    }
    vector& operator=(std::initializer_list<T> il)
    {
        destruct();
        _data = calloc<T>(il.size(), sizeof(T));
        _size = il.size();
        std::copy(il.begin(), il.end(), _data);
        return *this;
    }
public:
    void clear()
    {
        destruct();
        _data = nullptr;
        _size = 0;
    }
    void resize(size_t n)
    {
        if(n == 0)
        {
            destruct();
            _data = nullptr;
            _size = 0;
        } else{
            auto new_data = calloc<T>(n, sizeof(T));
            std::move(_data, _data + std::min(n, _size), new_data);
            destruct();
            _data = new_data;
            _size = n;
        }
    }
    void push_back(const T& v)
    {
        resize(_size+1);
        back() = v;
    }
    void push_front(const T& v)
    {
        auto new_data = calloc<T>(_size + 1, sizeof(T));
        std::move(_data, _data + _size, new_data + 1);
        destruct();
        _data = new_data;
        _size = _size + 1;
        front() = v;
    }
    void erase(size_t index)
    {
        vector<T> new_v(_size-1);
        std::move(begin(), begin() + index, new_v.begin());
        std::move(begin()+index+1, end(), new_v.begin() + index);
        *this = new_v;
    }
public:
    size_t size() const {
        return _size;
    }

    T* begin(){
        return _data;
    }
    const T* begin() const{
        return _data;
    }

    T* end()
    {
        return _data + _size;
    }
    const T* end() const
    {
        return _data + _size;
    }

    T* data()
    {
        return _data;
    }
    const T* data() const
    {
        return _data;
    }

    T& operator[](size_t i)
    {
        return _data[i];
    }
    const T& operator[](size_t i) const
    {
        return _data[i];
    }

    T& front()
    {
        return _data[0];
    }
    const T& front() const
    {
        return _data[0];
    }
    T& back()
    {
        return _data[_size-1];
    }
    const T& back() const
    {
        return _data[_size-1];
    }
};

STD_LIB_NAMESPACE_END