#include <cstdint>
#include <type_traits>
#include <utility>
namespace SK::Container::String {

template <char...>
struct ct_string;

template <typename, typename>
struct concat;

template <char... Cs1, char... Cs2>
struct concat<ct_string<Cs1...>, ct_string<Cs2...>> {
    using type = ct_string<Cs1..., Cs2...>;
};

template <char... Cs>
struct ct_string {
private:
    template <std::size_t P, std::size_t... Is>
    static consteval auto substr_impl(std::index_sequence<Is...>) {
        return std::type_identity<ct_string<Cs...[P + Is]...>>{};
    }

public:
    using self = ct_string<Cs...>;

    static constexpr std::size_t size = sizeof...(Cs);

    template <std::size_t I>
    static constexpr char at = Cs...[I];

    template <std::size_t P, std::size_t C>
    using substr = decltype(substr_impl<P>(std::make_index_sequence<C>{}))::type;

    template <typename Other>
    using append = concat<self, Other>::type;

    template <typename Other>
    static constexpr bool equal = 
        std::is_same_v<
            self,
            Other
        >;
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
    using A = cts<"abc"_cts>;
    using B = cts<"ab"_cts>;

    static_assert(A::equal<B::append<cts<"c"_cts>>>);

    std::cout << A::substr<1, 2>::at<0> <<std::endl;

    // using table = ct_table<
    //     ct_entry<A, type_array<
    //         double, int
    //     >>
    // >::add<B, double>;

    // static_assert(table::get<A>::has<bool>);
    // static_assert(std::is_same_v<table::get<B>, double>);
}