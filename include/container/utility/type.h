#pragma once

#include <type_traits>

#include "../../config/cpp_version.h"

namespace SK::Container::Utility {

template <typename Ty, typename... Tys>
struct is_any_of : 
#if CPP_STD >= CPP17
    std::disjunction<
        std::is_same<Ty, Tys>...
    > {};
#elif CPP_STD >= CPP11
    std::false_type {};
    template <typename Ty, typename First, typename... Rest>
    struct is_any_of<Ty, First, Rest...> : 
        std::conditional<
            std::is_same<Ty, First>::value,
            std::true_type,
            is_any_of<Ty, Rest...>
        >::type 
    {};
#endif

template <typename Ty, typename... Tys>
constexpr bool is_any_of_v
#if CPP_STD >= CPP14
    = is_any_of<Ty, Tys...>::value;
#else
    () { return is_any_of<Ty, Tys...>::value; }
#endif

template <typename CT>
constexpr bool is_character_type
#if CPP_STD >= CPP14
    = is_any_of_v<
        CT, char, wchar_t IF_CPP20(
            ,char8_t, char16_t, char32_t
        )
    >;
#else
    () { return is_any_of_v<CT, char, wchar_t>(); }
    
    template <typename CT>
    using enable_if_character = std::enable_if<is_character_type<CT>()>;

#if CPP_STD < CPP17
    template <typename...>
    using void_t = void;
#endif

    template <typename AT, typename ST, typename = void>
    struct is_allocator_type : std::false_type {};
    template <typename AT, typename ST>
    struct is_allocator_type<AT, ST,
        void_t<
            typename AT::value_type,
            decltype(std::declval<AT&>().allocate(std::declval<ST>())),
            decltype(std::declval<AT&>().deallocate(
                    std::declval<typename AT::value_type*>(),
                    std::declval<ST>()
                )
            )
        >
    > : std::true_type {};
    template <typename AT, typename ST>
    using enable_if_allocator = std::enable_if<is_allocator_type<AT, ST>::value>;

    template <typename UT, typename ST, typename = void>
    struct is_string_utility : std::false_type {};
    template <typename UT, typename ST>
    struct is_string_utility<UT, ST,
        void_t<
            decltype(std::declval<UT&>().strlen(
                std::declval<const typename UT::char_t*>()
            )),
            decltype(std::declval<UT&>().strcpy(
                std::declval<typename UT::char_t*>(),
                std::declval<const typename UT::char_t*>()
            )),
            decltype(std::declval<UT&>().strset(
                std::declval<typename UT::char_t*>(),
                std::declval<typename UT::char_t>(),
                std::declval<ST>()
            ))
        >
    > : std::true_type {};
    template <typename UT, typename ST>
    using enable_if_string_utility = std::enable_if<is_string_utility<UT, ST>::value>;

    template <typename ST>
    struct is_size_type : std::is_integral<ST> {};
#endif

IF_CPP20(
    template <typename CT>
    concept Character = is_character_type<CT>;
)

IF_CPP20(
    template <typename Alloc>
    concept Allocator = requires(Alloc alloc, std::size_t n) {
        typename Alloc::value_type;

        { alloc.allocate(n) } -> std::same_as<typename Alloc::value_type*>;
        { alloc.deallocate((typename Alloc::value_type*)nullptr, n) };
    };
)

IF_CPP20(
    template <typename U>
    concept StringUtility = requires(
        typename U::char_t* dst,
        const typename U::char_t* src,
        std::size_t n
    ) {
        { U::strcpy(dst, src) };
        { U::strset(dst, typename U::char_t{}, n) };
        { U::strcmp(src, src) } -> std::integral;
        { U::strlen(src) } -> std::integral;
    };
)

IF_CPP20(
    template <typename ST>
    concept SizeType = std::is_integral_v<ST>;
)

}