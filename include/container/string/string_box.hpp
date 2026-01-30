#pragma once

#include <cstdint>

#include "../../../foundation/types/str_types.h"

namespace SK::Container::String {

template <typename ST>
requires Utility::StringTraits<ST>
struct string_box {
    using string_traits = ST;
    
    using char_t            =                ST::char_t;
    using pointer_t         =             ST::pointer_t;

    static constexpr std::size_t char_size = sizeof(char_t);

    struct large_t {
        pointer_t   data;
        std::size_t size;
        std::size_t capacity : sizeof(std::size_t) * 8 - 1;
        std::size_t is_large :  1;
    };

    static constexpr std::size_t max_cache_size = sizeof(large_t) / char_size - 1;

    struct cache_t {
        char_t data[max_cache_size];
        Utility::char_integer<char_t>::type size : char_size * 8 - 1;
        Utility::char_integer<char_t>::type is_large : 1;
    };

    struct [[using gnu: aligned(sizeof(std::size_t))]] {
        union {
            cache_t cache;
            large_t large;
        };
    } storage;

    constexpr string_box() noexcept : storage {} {}
    constexpr ~string_box() noexcept = default;

};


}