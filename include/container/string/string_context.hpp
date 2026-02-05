#pragma once

#include <cstdint>
#include <type_traits>

namespace SK::Container::String {

enum class string_flag {
    None,
    Mutable,
    Shared,
    Noexcept,
    Backtracking
};

template <string_flag... Flags>
struct string_context;

template <string_flag... Flags>
struct unique_util;

template <>
struct unique_util<> {
    using type = string_context<>;
};

template <string_flag First, string_flag... Rest>
struct unique_util<First, Rest...> {
private:
    using tail_unique = typename unique_util<Rest...>::type;

public:
    using type = std::conditional_t<
        ((First == Rest) || ...),
        tail_unique,                       
        typename tail_unique::template with_flag<First>
    >;
};

template <string_flag RemoveFlag, string_flag... Flags>
struct filter_flags;

template <string_flag RemoveFlag>
struct filter_flags<RemoveFlag> {
    using type = string_context<>;
};

template <string_flag RemoveFlag, string_flag First, string_flag... Rest>
struct filter_flags<RemoveFlag, First, Rest...> {
private:
    using tail_filtered = typename filter_flags<RemoveFlag, Rest...>::type;
public:
    using type = std::conditional_t<
        RemoveFlag == First,
        tail_filtered,
        typename tail_filtered::template with_flag<First>
    >;
};

template <string_flag... Flags>
struct string_context {
    using self = unique_util<Flags...>::type;

    template <string_flag HasFlag>
    static constexpr bool has_v = ((Flags == HasFlag) || ...);

    template <string_flag... HasFlags>
    static constexpr bool has_all_v = (has_v<HasFlags> && ...);
    
private:
    template <string_flag NewFlags>
    using add_flag = string_context<Flags..., NewFlags>;

    template <string_flag NewFlag, bool Enable>
    using add_flag_if = 
        std::conditional_t<
            Enable,
            add_flag<NewFlag>,
            self
        >;
    
    template <string_flag NewFlag, bool Enable>
    using add_unique_flag = 
        std::conditional_t<
            has_v<NewFlag>,
            self,
            add_flag_if<NewFlag, Enable>
        >;

    template <string_flag RemoveFlag>
    using remove_flag = filter_flags<RemoveFlag, Flags...>::type;

    template <string_flag RemoveFlag, bool Enable>
    using remove_flag_if = 
        std::conditional_t<
            Enable,
            remove_flag<RemoveFlag>,
            self
        >;

public:
    consteval string_context() noexcept {}

    template <string_flag NewFlag, bool Enable = true>
    using with_flag = add_unique_flag<NewFlag, Enable>;

    template <string_flag RemoveFlag, bool Enable = true>
    using without_flag = remove_flag_if<RemoveFlag, Enable>;
};


}