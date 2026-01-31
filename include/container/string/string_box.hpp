#pragma once

#include <cstdint>

#include "../../../foundation/types/str_types.h"

namespace SK::Container::String {

template <typename ST>
//requires Utility::StringTraits<ST>
struct string_box {
    using string_traits = ST;
    
    using char_t            =                char;
    using pointer_t         =            char*;

    static constexpr std::size_t char_size = sizeof(char_t);

    struct large_t {
        pointer_t   data;
        std::size_t size;
        std::size_t capacity : sizeof(std::size_t) * 8 - 1;
        std::size_t is_large : 1;
    };

    static constexpr std::size_t max_cache_size = sizeof(large_t) / char_size - 1;

    struct cache_t {
        using size_type = Utility::char_integer<char_t>::type;

        char_t data[max_cache_size];
        size_type size : char_size * 8 - 1;
        size_type is_large : 1;
    };

    struct [[using gnu: aligned(sizeof(std::size_t))]] {
        union {
            cache_t cache;
            large_t large;
        };
    } storage;

    constexpr explicit string_box() noexcept 
        : storage {
            cache_t{}
        }
    {}

    constexpr explicit string_box(std::size_t size) noexcept 
        : storage{
            .cache {
                .data {},
                .size = static_cast<typename cache_t::size_type>(size),
                .is_large = 0
            }
        } 
    {
        if (size >= max_cache_size) {
            storage.cache.is_large = 1;
        }
    }
    
    constexpr ~string_box() noexcept = default;
};


}