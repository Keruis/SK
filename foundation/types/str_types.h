#pragma once

#include <type_traits>

#include "../config/str_optimization_level.h"

namespace SK::Container::Utility {

template <typename Ty, typename... Tys>
struct is_any_of :
    std::disjunction<
        std::is_same<Ty, Tys>...
    > {};

template <typename Ty, typename... Tys>
constexpr bool is_any_of_v
    = is_any_of<Ty, Tys...>::value;

template <typename CT>
constexpr bool is_character_type
    = is_any_of_v<
        CT, char, wchar_t, char8_t, char16_t, char32_t
    >;

template <typename CT>
struct char_integer;

template <>
struct char_integer<char> {
    using type = uint8_t;
};

template <>
struct char_integer<char8_t> {
    using type = uint8_t;
};

template <>
struct char_integer<wchar_t> {
    using type = uint16_t;
};

template <>
struct char_integer<char16_t> {
    using type = uint16_t;
};

template <>
struct char_integer<char32_t> {
    using type = uint32_t;
};

template <typename CT>
concept Character = is_character_type<CT>;

template <typename Alloc, typename CT>
concept Allocator = requires(Alloc alloc, std::size_t n) {
    typename Alloc::value_type;
    requires std::same_as<typename Alloc::value_type, CT>;
    { alloc.allocate(n) } -> std::same_as<typename Alloc::value_type*>;
    { alloc.deallocate((typename Alloc::value_type*)nullptr, n) };
};

template <typename SU>
concept StringUtility = requires(
    typename SU::char_t* dst,
    const typename SU::char_t* src,
    std::size_t n
) {
    { SU::strcpy(dst, src, n) };
    { SU::strset(dst, typename SU::char_t{}, n) };
    { SU::strcmp(src, src, n) } -> std::integral;
    { SU::strlen(src) } -> std::integral;
};

template <typename ST>
concept StringTraits = requires {
    typename ST::char_t;
    typename ST::reference_t;
    typename ST::const_reference_t;
    typename ST::pointer_t;
    typename ST::const_pointer_t;
    typename ST::alloc_t;
    typename ST::strutil;
    { ST::str_optimization } -> std::convertible_to<Config::str_optimization_level>;
};

}