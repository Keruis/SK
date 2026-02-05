#pragma once

#include <bit>
#include <limits>
#include <format>

#include "../../../foundation/types/str_types.h"
#include "../../../foundation/config/str_optimization_level.h"

#include "string_box.hpp"
#include "string_context.hpp"

namespace SK::Container::String {

template <
    typename SCTX,
    typename ST
> requires Utility::StringTraits<ST>
class string_core : protected string_box<ST> {
public:
    using string_context = typename SCTX::template with_flag<
        string_flag::Noexcept,
        requires {
            { std::declval<string_core&>().
                create(std::declval<std::size_t&>(),
                std::declval<std::size_t&>()) 
            } noexcept;
        }
    >;
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

    constexpr string_core(const string_core& other) 
        noexcept (
            SCTX::template has_v<string_flag::Noexcept>
        ) 
    {
        assign_init(other);
    }

    constexpr string_core(string_core&& other) noexcept {
        if (other.is_mode<string_mode::cache>()) {
            move_from_cache(other.storage);
            return;
        }

        move_from_large(other.storage);
    }

    constexpr string_core(char_t ch) 
        noexcept : box_t(ch)
    {}

    constexpr string_core(char_t ch, std::size_t size)
        noexcept (
            noexcept (
                std::declval<string_core&>()
                    .assign_init(std::declval<char_t>())
            )
        ) : box_t(size)
    {
        assign_init(ch);
    }

    constexpr string_core(const_pointer_t str) 
        noexcept (
            noexcept(
                std::declval<string_core&>()
                    .assign_init(std::declval<const_pointer_t>())
            )
        ) : box_t(strutil::strlen(str))
    {
        assign_init(str);
    }

    constexpr string_core(const_pointer_t str, std::size_t length) 
        noexcept (
            noexcept(
                std::declval<string_core&>()
                    .assign_init(std::declval<const_pointer_t>())
            )
        ) : box_t(length)
    {
        assign_init(str);
    }

    ~string_core() noexcept {
        if (is_mode<string_mode::large>()) {
            const auto capacity = pow2(static_cast<std::size_t>(storage.header.extent));
            alloc_t::deallocate(storage.data.large.data, capacity);
        }
    }

    [[nodiscard]] constexpr const_pointer_t data() const noexcept {
        if (is_mode<string_mode::cache>()) {
            return storage.data.cache.data;
        }
        return storage.data.large.data;
    }

    [[nodiscard]] constexpr pointer_t data() noexcept {
        return const_cast<pointer_t>(
            static_cast<const string_core*>(this)->data()
        );
    }

    [[nodiscard]] constexpr const_pointer_t c_str() const noexcept {
        return data();
    }

    [[nodiscard]] constexpr pointer_t c_str() noexcept {
        return data();
    }

    [[nodiscard]] constexpr std::size_t size() const noexcept {
        if (is_mode<string_mode::cache>()) {
            return static_cast<std::size_t>(storage.header.extent);
        }
        return storage.data.large.size;
    }

    [[nodiscard]] constexpr std::size_t length() const noexcept {
        return size();
    }

    [[nodiscard]] constexpr std::size_t capacity() const noexcept {
        if (is_mode<string_mode::cache>()) {
            return box_t::max_cache_size;
        }
        return pow2(storage.header.extent);
    }

    [[nodiscard]] constexpr bool empty() const noexcept {
        return size() == 0;
    }

    constexpr void clear() 
        noexcept (
            noexcept (
                alloc_t::deallocate(std::declval<pointer_t>(), std::declval<std::size_t>())
            )
        ) 
    {
        if (is_mode<string_mode::large>()) {
            alloc_t::deallocate(
                storage.data.large.data,
                pow2(storage.header.extent)
            );
        }
        reset();
    }

    [[nodiscard]] constexpr const char_t& at(std::size_t pos) const {
        if (pos >= size())
            throw std::out_of_range("string::at");
        
        return data()[pos];
    }

    [[nodiscard]] constexpr char_t& at(std::size_t pos) {
        return const_cast<char_t&>(
            static_cast<const string_core*>(this)->at(pos)
        );
    }

    [[nodiscard]] constexpr const char_t& front() const noexcept {
        return data()[0];
    }

    [[nodiscard]] constexpr char_t& front() noexcept {
        return const_cast<char_t&>(
            static_cast<const string_core*>(this)->front()
        );
    }

    [[nodiscard]] constexpr const char_t& back() const noexcept {
        return data()[size() - 1];
    }

    [[nodiscard]] constexpr char_t& back() noexcept {
        return const_cast<char_t&>(
            static_cast<const string_core*>(this)->back()
        );
    }

    constexpr void resize(std::size_t size) 
        noexcept (
            noexcept (
                std::declval<string_core&>()
                    .resize_cache(std::declval<std::size_t>())
            ) &&
            noexcept (
                std::declval<string_core&>()
                    .resize_large(std::declval<std::size_t>())
            )
        ) 
    {
        if (is_mode<string_mode::cache>()) {
            return resize_cache(size);
        }
        resize_large(size);
    }

    constexpr void reserve(std::size_t size) 
        noexcept (
            noexcept (
                std::declval<string_core&>()
                    .reserve_cache(std::declval<std::size_t>())
            ) &&
            noexcept (
                std::declval<string_core&>()
                    .reserve_large(std::declval<std::size_t>())
            )
        ) 
    {
        if (is_mode<string_mode::cache>()) {
            return reserve_cache(size);
        }
        reserve_large(size);
    }

private:
    template <string_mode mode>
    constexpr bool is_mode() const noexcept {
        return storage.header.mode == mode;
    }

    template <Config::str_optimization_level level>
    consteval bool is_level() const noexcept {
        return string_traits::str_optimization == level;
    }

    constexpr void reset() noexcept {
        storage.header.mode = string_mode::cache;
        storage.header.extent = 0;
        storage.data.cache.data[0] = char_t();
    }

    constexpr std::size_t pow2(std::size_t exp) const noexcept {
        return std::size_t{1} << exp;
    }

    constexpr std::size_t next_pow2_exp(std::size_t n) const noexcept {
        if (n <= 1) return 0;
        return std::bit_width(n - 1);
    }

    constexpr pointer_t create(std::size_t& capacity, std::size_t& exp)
        noexcept (
            is_level<Config::str_optimization_level::Ofast>() &&
            noexcept (
                alloc_t::allocate(std::declval<std::size_t>())
            )
        )
    {
        const auto rounded = pow2(exp = next_pow2_exp(capacity));

        if (rounded > max_size) {
            throw std::length_error(
                "string::create: size too big"
            );
        }

        return alloc_t::allocate((capacity = rounded));
    }

    constexpr void copy_from_cache(const box_t::storage_t& other) noexcept {
        storage = other;
    }

    constexpr void copy_from_large(const box_t::storage_t& other) 
        noexcept (
            noexcept (
                std::declval<string_core&>()
                    .create(std::declval<std::size_t&>(), std::declval<std::size_t&>())
            )
        )
    {
        const_pointer_t str_ptr = other.data.large.data;
        const std::size_t length = other.data.large.size;
        const std::size_t alloc_size = length + 1;
        std::size_t exp {};
        auto* alloc_ptr = create(alloc_size, exp);
        storage.header.mode = string_mode::large;
        storage.header.extent = static_cast<box_t::size_type>(exp);
        storage.data.large.size = length;
        storage.data.large.data = alloc_ptr;
        strutil::strcpy(alloc_ptr, str_ptr, length);
        alloc_ptr[length] = char_t();
    }

    constexpr void move_from_cache(box_t::storage_t& other) noexcept {
        storage = other;
        other.reset();
    }

    constexpr void move_from_large(box_t::storage_t& other) noexcept {
        storage = other;
        other.reset();
    }

    constexpr void copy_cache_str(std::size_t buf_size, const_pointer_t str, std::size_t size) noexcept {
        strutil::strcpy(
            storage.data.cache.data + buf_size,
            str,
            size
        );
    }

    constexpr void copy_large_str(std::size_t buf_size, const_pointer_t str, std::size_t size) noexcept {
        strutil::strcpy(
            storage.data.large.data + buf_size,
            str,
            size
        );
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
            auto& cache = storage.data.cache;
            const auto length = static_cast<std::size_t>(header.extent);
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
        auto alloc_size = length + 1;
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

    constexpr void assign_init(char_t ch)
        noexcept (
            noexcept (
                std::declval<string_core&>()
                    .create(std::declval<std::size_t&>(), std::declval<std::size_t&>())
            )
        )
    {
        if (is_mode<string_mode::cache>()) {
            const auto& header = storage.header;
            auto& cache = storage.data.cache;
            const auto length = static_cast<std::size_t>(header.extent);
            strutil::strset(
                cache.data, 
                ch,
                length
            );
            cache.data[length] = char_t();
            return;
        }

        auto& header = storage.header;
        auto& large  = storage.data.large;
        const auto length = large.size;
        auto alloc_size = length + 1;
        std::size_t exp {};
        large.data = create(alloc_size, exp);
        header.extent = static_cast<box_t::size_type>(exp);
        strutil::strset(
            large.data, 
            ch, 
            length
        );
        large.data[length] = char_t();
    }

    constexpr void assign_init(const string_core& other) 
        noexcept (
            noexcept (
                std::declval<string_core&>()
                    .create(std::declval<std::size_t&>(), std::declval<std::size_t&>())
            )
        )
    {
        if (other.is_mode<string_mode::cache>()) {
            copy_from_cache(other.storage);
            return;
        }

        copy_from_large(other.storage);
    }

    constexpr void heapify_cache(std::size_t size)
        noexcept (
            noexcept (
               std::declval<string_core&>()
                    .create(std::declval<std::size_t&>(), std::declval<std::size_t&>())
            )
        )
    {
        auto& header = storage.header;
        auto& cache = storage.data.cache;

        std::size_t length = static_cast<std::size_t>(header.extent);
        std::size_t exp {};
        std::size_t alloc_size = length + 1;
        auto* alloc_ptr = create(alloc_size, exp);
        strutil::strcpy(
            alloc_ptr,
            cache.data,
            length
        );
        alloc_ptr[length] = char_t();
        auto& large = storage.data.large;
        header.mode = string_mode::large;
        header.extent = static_cast<box_t::size_type>(exp);
        large.size = length;
        large.data = alloc_ptr;
    }

    constexpr void new_space(std::size_t size)
        noexcept (
            noexcept (
               std::declval<string_core&>()
                    .create(std::declval<std::size_t&>(), std::declval<std::size_t&>())
            )
        )
    {
        auto& header = storage.header;
        auto& large = storage.data.large;

        auto* old_ptr = storage.data.large.data;
        std::size_t exp {};
        std::size_t alloc_size = size + 1;
        auto* alloc_ptr = create(alloc_size, exp);
        strutil::strcpy(
            alloc_ptr,
            old_ptr,
            large.size
        );
        alloc_ptr[size] = char_t();
        alloc_t::deallocate(old_ptr, pow2(static_cast<std::size_t>(header.extent)));
        header.extent = static_cast<box_t::size_type>(exp);
        large.data = alloc_ptr;
    }

    template <bool init_heap>
    constexpr void respace(std::size_t size) 
        noexcept (
            noexcept (
                std::declval<string_core&>()
                    .heapify_cache(std::declval<std::size_t>())
            ) &&
            noexcept (
                std::declval<string_core&>()
                    .new_space(std::declval<std::size_t>())
            )
        ) 
    {
        if constexpr (init_heap) {
            return heapify_cache(size);
        }

        return new_space(size);
    }

    constexpr void resize_cache(std::size_t size) 
        noexcept (
            requires {
                { respace<true>(std::declval<std::size_t>()) } noexcept;
            }
        ) 
    {
        auto& header = storage.header;
        auto& cache = storage.data.cache;
        if (size < box_t::max_cache_size) {
            cache.data[size] = char_t();
            header.extent = static_cast<box_t::size_type>(size);
            return;
        }

        respace<true>(size);
    }

    constexpr void resize_large(std::size_t size) 
        noexcept (
            requires {
                { respace<false>(std::declval<std::size_t>()) } noexcept;
            }
        ) 
    {
        auto& header = storage.header;
        auto& large = storage.data.large;
        if (size < pow2(header.extent)) {
            large.data[size] = char_t();
            large.size = size;
            return;
        }

        respace<false>(size);
        large.size = size;
    }

    constexpr void reserve_cache(std::size_t size)
        noexcept (
            requires {
                { respace<true>(std::declval<std::size_t>()) } noexcept;
            }
        ) 
    {
        if (size < box_t::max_cache_size) return;

        respace<true>(size);
    }

    constexpr void reserve_large(std::size_t size)
        noexcept (
            requires {
                { respace<false>(std::declval<std::size_t>()) } noexcept;
            }
        ) 
    {
        auto& header = storage.header;
        if (size < pow2(static_cast<std::size_t>(header.extent))) return;

        respace<false>(size);
    }

    constexpr void append_cache(const_pointer_t str, std::size_t size) 
        noexcept (
            requires {
                { respace<false>(std::declval<std::size_t>()) } noexcept;
            }
        ) 
    {
        auto& header = storage.header;
        auto& cache = storage.data.cache;

        std::size_t length = static_cast<std::size_t>(header.extent);
        std::size_t next_length = length + size;
        if (next_length < box_t::max_cache_size) {
            copy_cache_str(
                length, 
                str, 
                size
            );
            header.extent = static_cast<box_t::size_type>(next_length);
            cache.data[next_length] = char_t();
            return;
        }
        
        respace<true>(next_length);
        auto& large = storage.data.large;
        copy_large_str(
            large.size,
            str,
            size
        );
        large.size = next_length;
        large.data[next_length] = char_t();
    }

    constexpr void append_large(const_pointer_t str, std::size_t size) 
        noexcept(
            requires {
                { respace<false>(std::declval<std::size_t>()) } noexcept;
            }
        )
    {
        auto& header = storage.header;
        auto& large = storage.data.large;

        std::size_t length = large.size;
        std::size_t next_length = length + size;
        if (next_length < pow2(static_cast<std::size_t>(header.extent))) {
            copy_large_str(
                length, 
                str, 
                size
            );
            large.size = next_length;
            large.data[next_length] = char_t();
            return;
        }

        respace<false>(next_length);
        copy_large_str(
            length, 
            str, 
            size
        );
        large.size = next_length;
        large.data[next_length] = char_t();
    }

protected:
    constexpr void append_impl(const_pointer_t str, std::size_t size) 
        noexcept(
            requires {
                { append_cache(str, size) } noexcept;
                { append_large(str, size) } noexcept;
            }
        )
    {
        if (is_mode<string_mode::cache>()) {
            return append_cache(str, size);
        }
        append_large(str, size);
    }

    constexpr void append_impl(char_t ch) 
        noexcept(
            requires {
                { append_impl(std::declval<const_pointer_t>(), 1) } noexcept;
            }
        )
    {
        const char_t* ptr = &ch;
        append_impl(ptr, 1);
    }

    constexpr void append_impl(const_pointer_t str) 
        noexcept(
            requires {
                { append_impl(str, strutil::strlen(str)) } noexcept;
            }
        )
    {
        append_impl(str, strutil::strlen(str));
    }

    constexpr void append_impl(const string_core& other) 
        noexcept(
            requires {
                { append_impl(other.data(), other.size()) } noexcept;
            }
        ) 
    {
        append_impl(other.data(), other.size());
    }
};

}