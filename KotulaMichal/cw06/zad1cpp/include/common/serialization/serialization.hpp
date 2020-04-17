#pragma once

#include <common/common.hpp>

template<class T>
class serialization
{
public:
    static size_t calculate_serialized_size(const T& v)
    {
        return sizeof(T);
    }
    static byte_buffer_iterator serialize(const T& v, byte_buffer_iterator offset)
    {
        return copy(reinterpret_cast<const byte*>(&v), reinterpret_cast<const byte*>(&v) + sizeof(T), offset);
    }
    static T deserialize(byte_buffer_iterator& offset)
    {
        T value;
        std::copy(offset, offset + sizeof(T), reinterpret_cast<byte*>(&value));
        offset +=  sizeof(T);
        return value;
    }
};

template<class T>
class serialization<vector<T>>
{
public:
    static size_t calculate_serialized_size(const vector<T>& v)
    {
        size_t size = sizeof(v.size());
        for(auto& e : v)
        {
            size += serialization<T>::calculate_serialized_size(e);
        }
        return size;
    }
    static byte_buffer_iterator serialize(const vector<T>& v, byte_buffer_iterator offset)
    {
        offset = serialization<size_t>::serialize(v.size(), offset);
        for(auto& e : v)
        {
            offset = serialization<T>::serialize(e, offset);
        }
        return offset;
    }
    static vector<T> deserialize(byte_buffer_iterator& offset)
    {
        vector <T> data;
        auto size = serialization<size_t>::deserialize(offset);
        data.reserve(size);
        for (size_t i = 0; i < size; i++) {
            data.emplace_back(serialization<T>::deserialize(offset));
        }
        return data;
    }
};

template<>
class serialization<string_view>
{
public:
    static size_t calculate_serialized_size(const string_view& v)
    {
        return sizeof(size_t) + v.size();
    }
    static byte_buffer_iterator serialize(const string_view& v, byte_buffer_iterator offset)
    {
        offset = serialization<size_t>::serialize(v.size(), offset);
        return std::copy(v.begin(), v.end(), offset);
    }
    static string_view deserialize(byte_buffer_iterator& offset)
    {
        throw runtime_error("Cannot deserialize into string_view. Deserialize into string instead.");
    }
};

template<>
class serialization<string>
{
public:
    static size_t calculate_serialized_size(const string& v)
    {
        return sizeof(size_t) + v.size();
    }
    static byte_buffer_iterator serialize(const string& v, byte_buffer_iterator offset)
    {
        offset = serialization<size_t>::serialize(v.size(), offset);
        return std::copy(v.begin(), v.end(), offset);
    }
    static string deserialize(byte_buffer_iterator& offset)
    {
        string str;
        str.resize(serialization<size_t>::deserialize(offset));
        std::copy(offset, offset + str.size(), str.begin());
        offset += str.size();
        return str;
    }
};

template<class T1, class T2>
class serialization<pair<T1, T2>>
{
public:
    static size_t calculate_serialized_size(const pair<T1, T2>& v)
    {
        return serialization<T1>::calculate_serialized_size(v.first) + serialization<T2>::calculate_serialized_size(v.second);
    }
    static byte_buffer_iterator serialize(const pair<T1, T2>& v, byte_buffer_iterator offset)
    {
        offset = serialization<T1>::serialize(v.first, offset);
        return serialization<T2>::serialize(v.second, offset);
    }
    static pair<T1, T2> deserialize(byte_buffer_iterator& offset)
    {
        T1 v1 = serialization<T1>::deserialize(offset);
        T2 v2 = serialization<T2>::deserialize(offset);
        return pair<T1, T2>(std::move(v1), std::move(v2));
    }
};

template <typename Tuple, typename F, std::size_t ...Indices>
void for_each_impl(Tuple&& tuple, F&& f, std::index_sequence<Indices...>) {
    using swallow = int[];
    (void)swallow{1,
                  (f(std::get<Indices>(std::forward<Tuple>(tuple))), void(), int{})...
    };
}

template <typename Tuple, typename F>
void for_each(Tuple&& tuple, F&& f) {
    constexpr std::size_t N = std::tuple_size<std::remove_reference_t<Tuple>>::value;
    for_each_impl(std::forward<Tuple>(tuple), std::forward<F>(f),
                  std::make_index_sequence<N>{});
}

template<class... TS>
class serialization<tuple<TS...>>
{
public:
    static size_t calculate_serialized_size(const tuple<TS...>& v)
    {
        size_t size = 0;
        for_each(v, [&size](const auto& tv){
            size += serialization<remove_reference_t<decltype(tv)>>::calculate_serialized_size(tv);
        });
        return size;
    }
    static byte_buffer_iterator serialize(const tuple<TS...>& v, byte_buffer_iterator offset)
    {
        for_each(v, [&offset](const auto& tv){
            offset = serialization<remove_reference_t<decltype(tv)>>::serialize(tv, offset);
        });
        return offset;
    }
    static tuple<TS...> deserialize(byte_buffer_iterator& offset)
    {
        return {serialization<TS>::deserialize(offset)... };
    }
};


