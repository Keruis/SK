#include <cstdint>
#include <type_traits>
#include <utility>

namespace SK::Container::String {

template <char...>
struct ct_string;

template <typename, typename>
struct ct_concat;

template <char... Cs1, char... Cs2>
struct ct_concat<ct_string<Cs1...>, ct_string<Cs2...>> {
    using type = ct_string<Cs1..., Cs2...>;
};

template <std::size_t, typename, typename>
struct ct_find;

template <char... Cs1, char... Cs2>
struct ct_find<ct_string<Cs1...>::size - ct_string<Cs2...>::size + 1, ct_string<Cs1...>, ct_string<Cs2...>> {
    static constexpr std::size_t pos = static_cast<std::size_t>(-1);
};

template <std::size_t N, char... Cs1, char... Cs2>
struct ct_find<N, ct_string<Cs1...>, ct_string<Cs2...>> {
    static constexpr std::size_t pos = 
        (N + ct_string<Cs2...>::size > ct_string<Cs1...>::size)
        ? static_cast<std::size_t>(-1)
        : 
        (
            ct_string<Cs1...>::template substr<N, ct_string<Cs2...>::size>::template equal<ct_string<Cs2...>>
            ? N
            : ct_find<N + 1, ct_string<Cs1...>, ct_string<Cs2...>>::pos
        );
};

template <std::size_t, typename, std::size_t...>
struct ct_substr;

template <std::size_t P, char... Cs, std::size_t... Is>
struct ct_substr<P, ct_string<Cs...>, Is...> {
    using type = ct_string<Cs...[P + Is]...>;
};

template <char... Cs>
struct ct_string {
    using self = ct_string<Cs...>;

    static constexpr std::size_t size = sizeof...(Cs);

    static constexpr bool empty = size == 0;

    template <std::size_t I>
    static constexpr char at = Cs...[I];

    template <std::size_t P, std::size_t C>
    using substr = ct_substr<P, self, __integer_pack(C)...>::type;

    template <typename Other>
    using append = ct_concat<self, Other>::type;

    template <typename Other>
    static constexpr bool equal = 
        std::is_same_v<
            self,
            Other
        >;

   template <typename Other, std::size_t S = 0>
   static constexpr std::size_t find = ct_find<S, self, Other>::pos;

};

template <typename CT, CT... Cs>
consteval auto operator""_cts() {
    return std::type_identity<ct_string<Cs...>>{};
}

template <auto CTST>
using cts = typename decltype(CTST)::type;


}

using namespace SK::Container::String;

#include <iostream>

int main() {
    using A = cts<"abcd"_cts>;
    using B = cts<"ab"_cts>;

    static_assert(A::equal<B::append<cts<"cd"_cts>>>);
    static_assert(A::substr<1, 2>::at<0> == 'b');
    static_assert(A::size == 4);
    static_assert(A::find<cts<"cd"_cts>> == 2);
}