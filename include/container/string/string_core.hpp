#pragma once

#include <bit>
#include <limits>
#include <format>

#include "../../../foundation/types/str_types.h"
#include "../../../foundation/config/str_optimization_level.h"

#include "string_box.hpp"

namespace SK::Container::String {

template <
    typename ST,
    bool IsShared = false
> requires Utility::StringTraits<ST>
class string_core : protected string_box<ST> {
public:
    using string_traits = ST;

    using char_t            =   typename string_traits::char_t;
    using reference_t       =   typename string_traits::reference_t;
    using const_reference_t =   typename string_traits::const_reference_t;
    using pointer_t         =   typename string_traits::pointer_t;
    using const_pointer_t   =   typename string_traits::const_pointer_t;

    static constexpr std::size_t max_size = std::numeric_limits<std::size_t>::max() - 1;

protected:
    using box_t = string_box<string_traits>;
    using box_t::box_t;
    using box_t::storage;

private:
    using alloc_t = typename string_traits::alloc_t;
    using strutil = typename string_traits::strutil;

public:
    constexpr string_core() noexcept
        : box_t()
    {}

    constexpr string_core(char_t ch) noexcept
        : box_t(ch)
    {}

    constexpr string_core(const_pointer_t str) noexcept
        : box_t(strutil::strlen(str))
    {
        assign_init(str);
    }

    ~string_core() noexcept {

    }

    [[nodiscard]] constexpr const_pointer_t data() const noexcept {
        if (is_mode<string_mode::cache>()) {
            return storage.data.cache.data;
        }
        return storage.data.large.data;
    }

    [[nodiscard]] constexpr std::size_t size() const noexcept {
        if (is_mode<string_mode::cache>()) {
            return static_cast<std::size_t>(storage.header.extent);
        }
        return storage.data.large.size;
    }

    [[nodiscard]] constexpr std::size_t capacity() const noexcept {
        if (is_mode<string_mode::cache>()) {
            return box_t::max_cache_size;
        }
        return pow2(storage.header.extent);
    }

private:
    template <string_mode mode>
    [[nodiscard]] constexpr bool is_mode() const noexcept {
        return storage.header.mode == mode;
    }

    template <Config::str_optimization_level level>
    [[nodiscard]] consteval bool is_level() const noexcept {
        return string_traits::str_optimization == level;
    }

    [[nodiscard]] constexpr std::size_t pow2(std::size_t exp) const noexcept {
        return std::size_t{1} << exp;
    }

    [[nodiscard]] constexpr std::size_t next_pow2_exp(std::size_t n) const noexcept {
        if (n <= 1) return 0;
        return std::bit_width(n - 1);
    }

    [[nodiscard]] constexpr pointer_t create(std::size_t& capacity, std::size_t& exp)
        noexcept (
            is_level<Config::str_optimization_level::Ofast>() &&
            noexcept (
                alloc_t::allocate(std::declval<std::size_t>())
            )
        )
    {
        const auto rounded = pow2(exp = next_pow2_exp(capacity));

        if constexpr (!is_level<
            Config::str_optimization_level::Ofast
        >()) {
            if (rounded > max_size) {
                throw std::length_error(
                    "string::create: size too big"
                );
            }
        }

        return alloc_t::allocate((capacity = rounded));
    }

    constexpr void assign_init(const_pointer_t str) 
        noexcept (
            noexcept (
                std::declval<string_core&>()
                    .create(std::declval<std::size_t&>(), std::declval<std::size_t&>())
            )
        ) 
    {
        if (is_mode<string_mode::cache>()) {
            const auto& header = storage.header;
            auto& cache  = storage.data.cache;
            const auto length = static_cast<std::size_t>(header.extent);
            [[assume(length <= box_t::max_cache_size)]]
            strutil::strcpy(
                cache.data, 
                str, 
                length
            );
            cache.data[length] = char_t();
            return;
        }

        auto& header = storage.header;
        auto& large  = storage.data.large;
        const auto length = large.size;
        auto alloc_size = length + (length >> 2);
        std::size_t exp {};
        large.data = create(alloc_size, exp);
        header.extent = static_cast<box_t::size_type>(exp);
        strutil::strcpy(
            large.data, 
            str, 
            length
        );
        large.data[length] = char_t();
    }

    
};

}