#pragma once

#include <cstdint>
#include <limits>

#include "../../../foundation/types/str_types.h"

namespace SK::Container::String {

template <typename ST>
//requires Utility::StringTraits<ST>
struct string_box {
    using string_traits = ST;
    
    using char_t            =                char;
    using pointer_t         =            char*;

    using size_type = Utility::char_integer<char_t>::type;

    static constexpr std::size_t char_size = sizeof(char_t);

    struct [[using gnu: packed]] large_t {
        pointer_t   data;
        std::size_t size;
        uint8_t _[8 - char_size];
    };

    static constexpr std::size_t max_cache_size = 3 * sizeof(std::size_t) / char_size - 1;

    struct cache_t {
        char_t data[max_cache_size];
    };

    struct [[using gnu: aligned(sizeof(std::size_t))]] storage_t {
        struct {
            size_type extent : char_size * 8 - 1;
            size_type is_large : 1;
        } header;
        struct {
            union {
                cache_t cache;
                large_t large;
            };
        } data;
    } storage;

    constexpr explicit string_box() noexcept 
        : storage {}
    {}

    constexpr explicit string_box(std::size_t size) noexcept 
        : storage {
            size >= max_cache_size ? storage_t {
                .header {
                    .is_large {1}
                },
                .data {
                    .large {
                        .size = size,
                    }
                }
            } : storage_t {
                .header {
                    .extent {
                        static_cast<
                            size_type
                        >(size) 
                    }
                }
            }
        }
    {
        
    }
    
    constexpr ~string_box() noexcept = default;
};


}