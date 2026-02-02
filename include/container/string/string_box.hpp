#pragma once

#include <cstdint>
#include <limits>

#include "../../../foundation/types/str_types.h"

namespace SK::Container::String {

enum class string_mode : bool {
    cache,
    large 
};

template <typename ST>
requires Utility::StringTraits<ST>
struct string_box {
    using string_traits = ST;
    
    using char_t            =                ST::char_t;
    using pointer_t         =             ST::pointer_t;

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
            string_mode mode : 1;
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
                    .mode { string_mode::large }
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
    {}

    constexpr explicit string_box(char_t ch) noexcept 
        : storage {
            .header {
                .extent {
                    1
                }
            },
            .data {
                .cache {
                    .data { ch }
                }
            }
        }
    {}
    
    constexpr ~string_box() noexcept = default;
};


}