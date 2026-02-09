#include <cstdint>
#include <type_traits>
#include <utility>

namespace SK::Container::String {

template <char...>
struct ct_string;

template <typename...>
struct ct_t_array;

template <typename... Entries>
struct ct_table;

template <typename, typename>
struct ct_concat;

template <char... Cs1, char... Cs2>
struct ct_concat<ct_string<Cs1...>, ct_string<Cs2...>> {
    using type = ct_string<Cs1..., Cs2...>;
};

template <typename... Ts1, typename... Ts2>
struct ct_concat<ct_t_array<Ts1...>, ct_t_array<Ts2...>> {
    using type = ct_t_array<Ts1..., Ts2...>;
};

template <std::size_t, typename, typename>
struct ct_find;

template <char... Cs1, char... Cs2>
struct ct_find<ct_string<Cs1...>::size - ct_string<Cs2...>::size + 1, ct_string<Cs1...>, ct_string<Cs2...>> {
    static constexpr std::size_t pos = static_cast<std::size_t>(-1);
};

template <typename SearchT, typename... Ts>
struct ct_find<ct_t_array<Ts...>::size, SearchT, ct_t_array<Ts...>> {
    static constexpr std::size_t pos = static_cast<std::size_t>(-1);
};

template <typename Key, typename... Entries>
struct ct_find<ct_table<Entries...>::size, Key, ct_table<Entries...>> {
    static_assert("ct_table::get<Key>: Key not found in table");
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

template <std::size_t N, typename SearchT, typename... Ts>
struct ct_find<N, SearchT, ct_t_array<Ts...>> {
    static constexpr std::size_t pos = 
        (N + 1 > ct_t_array<Ts...>::size)
        ? static_cast<std::size_t>(-1)
        : 
        (
            std::is_same_v<SearchT, Ts...[N]>
            ? N
            : ct_find<N + 1, SearchT, ct_t_array<Ts...>>::pos
        );
};

template<bool Cond, typename T, typename F>
struct lazy_conditional {
    using type = T;
};

template<typename T, typename F>
struct lazy_conditional<false, T, F> {
    using type = typename F::type;
};

template <std::size_t I, typename Key, typename... Entries>
struct ct_find<I, Key, ct_table<Entries...>> {
    using entry = typename ct_table<Entries...>::template at<I>;
    using type = lazy_conditional<
        std::is_same_v<Key, typename entry::key>,
        typename entry::value,
        ct_find<I + 1, Key, ct_table<Entries...>>
    >::type;
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

template <typename, typename, std::size_t>
struct ct_remove_t;

template <typename A, typename... Ts>
struct ct_remove_t<A, ct_t_array<Ts...>, ct_t_array<Ts...>::size> {
    using type = ct_t_array<>;
};

template <typename RemoveT, typename... Ts, std::size_t I>
struct ct_remove_t<RemoveT, ct_t_array<Ts...>, I> {
    using type = 
        std::conditional_t<
            std::is_same_v<RemoveT, Ts...[I]>,
            typename ct_remove_t<RemoveT, ct_t_array<Ts...>, I + 1>::type,
            typename ct_remove_t<RemoveT, ct_t_array<Ts...>, I + 1>::type::template add<Ts...[I]>
        >;
};

template <typename A, typename... Entries>
struct ct_remove_t<A, ct_table<Entries...>, ct_table<Entries...>::size> {
    using type = ct_table<>;
};

template <typename RemoveKey, typename... Entries, std::size_t I>
struct ct_remove_t<RemoveKey, ct_table<Entries...>, I> {
    using type = 
        std::conditional_t<
            std::is_same_v<RemoveKey, typename Entries...[I]::key>,
            typename ct_remove_t<RemoveKey, ct_table<Entries...>, I + 1>::type,
            typename ct_remove_t<RemoveKey, ct_table<Entries...>, I + 1>::type::template add<Entries...[I]>
        >;
};

template <typename, std::size_t>
struct ct_at;

template <std::size_t I, typename... Ts>
struct ct_at<ct_t_array<Ts...>, I> {
    using type = Ts...[I];
};

template <std::size_t I, typename... Entries>
struct ct_at<ct_table<Entries...>, I> {
    using type = Entries...[I];
};

template <typename... Ts>
struct ct_t_array {
    using self = ct_t_array<Ts...>;

    static constexpr std::size_t size = sizeof...(Ts);

    static constexpr bool empty = size == 0;

    template <std::size_t I>
    using at = ct_at<self, I>::type;

    template <typename NewT>
    using add = ct_t_array<Ts..., NewT>;

    template <typename Other>
    using concat = ct_concat<self, Other>::type;

    template <typename RemoveT, std::size_t S = 0>
    using remove = ct_remove_t<RemoveT, self, S>::type;

    template <typename T>
    static constexpr bool has = (std::is_same_v<T, Ts> || ...);

    template <typename... Us>
    static constexpr bool has_all = (has<Us> && ...);

    template <typename SearchT, std::size_t S = 0>
    static constexpr std::size_t find = ct_find<0, SearchT, self>::pos;

    template <typename L>
    static consteval void for_each(L lambda) {
        lambda.template operator()<Ts...>();
    }
};

template <typename Key, typename Value>
struct ct_entry {
    using key = Key;
    using value = Value;
};

template <typename... Entries>
struct ct_table {
    using self = ct_table<Entries...>;

    static constexpr std::size_t size = sizeof...(Entries);

    static constexpr bool empty = size == 0;

    template <std::size_t I>
    using at = ct_at<self, I>::type;

    template <typename SearchKey>
    static constexpr bool contains = (std::is_same_v<SearchKey, typename Entries::key> || ...);

    template <typename NewEntry>
    using add = ct_table<Entries..., NewEntry>;

    template <typename RemoveKey, std::size_t S = 0>
    using remove = ct_remove_t<RemoveKey, self, S>::type;

    template <typename SearchKey>
    static constexpr bool has = (std::is_same_v<SearchKey, typename Entries::key> || ...);

    template <typename Key, std::size_t S = 0>
    using get = ct_find<S, Key, self>::type;
};

}

using namespace SK::Container::String;

#include <iostream>

template <typename T>
struct print_t {
    print_t() {
        std::cout << __PRETTY_FUNCTION__ <<std::endl;
    }
};

int main() {
    using A = cts<"abcd"_cts>;
    using B = cts<"ab"_cts>;

    static_assert(A::equal<B::append<cts<"cd"_cts>>>);
    static_assert(A::substr<1, 2>::at<0> == 'b');
    static_assert(A::size == 4);
    static_assert(A::find<cts<"cd"_cts>> == 2);

    using C = ct_t_array<int, double>;
    static_assert(std::is_same_v<C::at<0>, int>);
    static_assert(C::size == 2);
    static_assert(std::is_same_v<C::add<char>, ct_t_array<int, double, char>>);
    static_assert(std::is_same_v<C::concat<ct_t_array<char>>, ct_t_array<int, double, char>>);
    static_assert(std::is_same_v<C::remove<double>, ct_t_array<int>>);
    static_assert(C::has<int>);
    static_assert(C::find<double> == 1);
    C::for_each(
        [] <typename... Ts> () {
            static_assert(
                std::is_same_v<Ts...[0], int> &&
                std::is_same_v<Ts...[1], double>
            );
        }
    );

    using D = ct_table<
        ct_entry<int, char>,
        ct_entry<double, bool>
    >;

    static_assert(D::size == 2);
    static_assert(std::is_same_v<D::get<int>, char>);
    static_assert(std::is_same_v<D::remove<int>, ct_table<
        ct_entry<double, bool>
    >>);
    static_assert(D::has<int>);
}