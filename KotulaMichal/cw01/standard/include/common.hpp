#pragma once

#include <includes.hpp>

STD_LIB_NAMESPACE_BEGIN

class string;
template<class T>
class vector;
template<class T1, class T2>
struct pair;

template<class T>
void free(T* ptr)
{
    //must call destructor before free if type has custom destructor
    if (!std::is_trivially_destructible<T>::value)
    {
        ptr->~T();
    }
    std::free(ptr);
}
template<class T>
void freen(T* arr_ptr, size_t n)
{
    //must call destructor before free if array type has custom destructor
    if (!std::is_trivially_destructible<T>::value)
    {
        auto b = arr_ptr;
        auto e = arr_ptr + n;

        while(b!=e)
        {
            (*b).~T();
            b++;
        }
    }

    std::free(arr_ptr);
}
template<class T>
T* calloc(size_t n, size_t objs)
{
    if(!n)
    {
        return nullptr;
    }

    auto ptr = (T*)std::calloc(n,objs);
    //must call default constructor before free if type has custom destructor
    if (!std::is_trivially_default_constructible<T>::value)
    {
        auto b = ptr;
        auto e = ptr + n;
        while(b!=e)
        {
            new (b) T(); //inplace constructor call (this is not memory allocation!)
            b++;
        }
    }

    return ptr;
}

STD_LIB_NAMESPACE_END