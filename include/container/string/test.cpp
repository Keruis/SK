// #include <iostream>
// #include <type_traits>
// #include "string_box.hpp"
// #include "string_traits.hpp"
// #include "string_core.hpp"
// #include "../utility/character.h"
// #include "../../../foundation/config/str_optimization_level.h"
// #include <cstddef>
// #include <new>
// #include <string>
// #include "string_basic.hpp"

// template <typename T>
// struct simple_allocator {
//     using value_type = T;

//     constexpr simple_allocator() noexcept = default;

//     static constexpr T* allocate(std::size_t n) {
//         if (n == 0) return nullptr;
//         return static_cast<T*>(::operator new(n * sizeof(T)));
//     }

//     static constexpr void deallocate(T* p, std::size_t) {
//         ::operator delete(p);
//     }
// };

// int main() {
//     using MyString = SK::Container::String::string_basic<
//         SK::Container::String::string_traits<
//             SK::Config::str_optimization_level::O0,
//             char,
//             simple_allocator<char>,
//             SK::Container::Utility::strutil
//         >
//     >;

//     std::cout << "String size = " << sizeof(MyString) << std::endl;

//     MyString s1('a', 5);
//     std::cout << s1.data() << std::endl;
//     std::cout << s1.size() << std::endl;
//     std::cout << s1.capacity() <<std::endl;

//     s1.append('b', "abc", MyString{}.append('r'));
//     std::cout << s1.data() << std::endl;
//     std::cout << s1.size() << std::endl;
//     std::cout << s1.capacity() <<std::endl;
//     return 0;
// }


// #include <iostream>
// #include <string>
// #include <chrono>
// #include <cstddef>
// #include <new>
// #include <type_traits>
// #include <utility>
// #include <windows.h>

// /* ===================== 你的 allocator ===================== */

// template <typename T>
// struct simple_allocator {
//     using value_type = T;

//     constexpr simple_allocator() noexcept = default;

//     static T* allocate(std::size_t n) {
//         if (n == 0) return nullptr;
//         return static_cast<T*>(::operator new(n * sizeof(T)));
//     }

//     static void deallocate(T* p, std::size_t) {
//         ::operator delete(p);
//     }
// };

// /* ===================== 你的 String ===================== */
// /* 你已有的头文件 */
// #include "string_box.hpp"
// #include "string_traits.hpp"
// #include "string_core.hpp"
// #include "../utility/character.h"
// #include "../../../foundation/config/str_optimization_level.h"
// #include "string_basic.hpp"
// #include "string_context.hpp"

// using MyString = SK::Container::String::string_basic<
//     SK::Container::String::string_context<>,
//     SK::Container::String::string_traits<
//         SK::Config::str_optimization_level::Ofast,
//         char,
//         simple_allocator<char>,
//         SK::Container::Utility::strutil
//     >
// >;

// /* ===================== 防止优化 ===================== */

// template <typename T>
// inline void do_not_optimize(T const& value) {
//     asm volatile("" : : "g"(value) : "memory");
// }

// /* ===================== 计时工具 ===================== */

// using m_clock_t = std::chrono::steady_clock;

// double seconds(m_clock_t::time_point start, m_clock_t::time_point end) {
//     return std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
// }

// /* ===================== 测试参数 ===================== */

// constexpr std::size_t N = 5'000'000;

// /* ===================== std::string ===================== */

// void test_std_string() {
//     volatile std::size_t sink = 0;

//     // 构造
//     {
//         auto t0 = m_clock_t::now();
//         for (std::size_t i = 0; i < N; ++i) {
//             std::string s(32, 'a');
//             do_not_optimize(s);
//         }
//         auto t1 = m_clock_t::now();
//         std::cout << "[std::string] ctor:      " << seconds(t0, t1) << " s\n";
//     }

//     // append
//     {
//         std::string s;
//         auto t0 = m_clock_t::now();
//         for (std::size_t i = 0; i < N; ++i) {
//             s.append("abc");
//             do_not_optimize(s);
//         }
//         auto t1 = m_clock_t::now();
//         std::cout << "[std::string] append:    " << seconds(t0, t1) << " s\n";
//     }

//     // capacity
//     {
//         std::string s(128, 'x');
//         auto t0 = m_clock_t::now();
//         for (std::size_t i = 0; i < N; ++i) {
//             sink += s.capacity();
//         }
//         auto t1 = m_clock_t::now();
//         do_not_optimize(sink);
//         std::cout << "[std::string] capacity:  " << seconds(t0, t1) << " s\n";
//     }

//     // resize
//     {
//         std::string s;
//         auto t0 = m_clock_t::now();
//         for (std::size_t i = 0; i < N; ++i) {
//             s.resize(i % 64);
//             do_not_optimize(s);
//         }
//         auto t1 = m_clock_t::now();
//         std::cout << "[std::string] resize:    " << seconds(t0, t1) << " s\n";
//     }
// }

// /* ===================== MyString ===================== */

// void test_my_string() {
//     volatile std::size_t sink = 0;

//     // 构造
//     {
//         auto t0 = m_clock_t::now();
//         for (std::size_t i = 0; i < N; ++i) {            MyString s('a', 32);
//             do_not_optimize(s);
//         }
//         auto t1 = m_clock_t::now();
//         std::cout << "[MyString]    ctor:      " << seconds(t0, t1) << " s\n";
//     }

//     // append
//     {
//         MyString s;
//         auto t0 = m_clock_t::now();
//         for (std::size_t i = 0; i < N; ++i) {
//             s.append("abc");
//             do_not_optimize(s);
//         }
//         auto t1 = m_clock_t::now();
//         std::cout << "[MyString]    append:    " << seconds(t0, t1) << " s\n";
//     }

//     // capacity
//     {
//         MyString s('x', 128);
//         auto t0 = m_clock_t::now();
//         for (std::size_t i = 0; i < N; ++i) {
//             sink += s.capacity();
//         }
//         auto t1 = m_clock_t::now();
//         do_not_optimize(sink);
//         std::cout << "[MyString]    capacity:  " << seconds(t0, t1) << " s\n";
//     }

//     // resize
//     {
//         MyString s;
//         auto t0 = m_clock_t::now();
//         for (std::size_t i = 0; i < N; ++i) {
//             s.resize(i % 64);
//             do_not_optimize(s);
//         }
//         auto t1 = m_clock_t::now();
//         std::cout << "[MyString]    resize:    " << seconds(t0, t1) << " s\n";
//     }
// }

// /* ===================== main ===================== */

// int main() {
//     std::cout << "sizeof(std::string) = " << sizeof(std::string) << "\n";
//     std::cout << "sizeof(MyString)    = " << sizeof(MyString) << "\n\n";

//     test_std_string();
//     std::cout << "---------------------------------\n";
//     test_my_string();
// size_t r = 0;

// auto stime = GetTickCount64();

// {
//     for (size_t i = 0; i < 1000000000; i++) {
//         std::string str = "Hello";
//         str.append(" World");
//         r += str.size();
//         do_not_optimize(str);
//     }
// }

// auto etime = GetTickCount64();

// std::cout << "Mine: " << (etime - stime) << '\n';
// std::cout << r << '\n';
// }


// #include "string_context.hpp"

// using namespace SK::Internal::String;

// int main() {
//     using ctx1 = string_context<string_flag::Mutable, string_flag::Mutable, string_flag::Shared>;
//     static_assert(ctx1::has_v<string_flag::Mutable>);
//     static_assert(ctx1::has_v<string_flag::Shared>);

//     using ctx2 = ctx1::without_flag<string_flag::Mutable>;
//     static_assert(ctx2::has_v<string_flag::Mutable>);
//     static_assert(ctx2::has_v<string_flag::Shared>);
// }

#include <iostream>
#include <utility>

template <char... CS>
struct ct_string;

template <typename S1, typename S2>
struct concat;

template <char... CS1, char... CS2>
struct concat<ct_string<CS1...>, ct_string<CS2...>> {
    using type = ct_string<CS1..., CS2...>;
};

template <char... CS>
struct ct_string {
    using self = ct_string<CS...>;

    static constexpr std::size_t size = sizeof...(CS);

    template <typename Other>
    static constexpr bool equal = 
        std::is_same_v<
            self,
            Other
        >;
    
    template <char... NewCS>
    using with_cs = ct_string<CS..., NewCS...>;

    template <typename Other>
    using with_cts = concat<self, Other>::type;
};

template <typename T, T... Cs>
consteval auto operator""_cts() {
    return std::type_identity<ct_string<Cs...>>{};
}

template <auto T>
using cts = typename decltype(T)::type;

template <typename Key, typename Value>
struct ct_entry {
    using key = Key;
    using value = Value;
};

template <typename SearchKey, typename... Entries>
struct ct_table_get_impl;

template <typename SearchKey>
struct ct_table_get_impl<SearchKey> {
    using type = void;
};

template <typename SearchKey, typename FirstEntry, typename... RestEntry>
struct ct_table_get_impl<SearchKey, FirstEntry, RestEntry...> {
    using type = std::conditional_t<
        SearchKey::template equal<typename FirstEntry::key>,
        typename FirstEntry::value,
        typename ct_table_get_impl<SearchKey, RestEntry...>::type
    >;
};

template <typename... Entries>
struct ct_table {
    template <typename SearchKey>
    using get = typename ct_table_get_impl<SearchKey, Entries...>::type;

    template <typename SearchKey, typename Value>
    static constexpr bool has = std::is_same_v<get<SearchKey>, Value>;

    template <typename Key, typename Value>
    using add = ct_table<Entries..., ct_entry<Key, Value>>;
};

template <typename... Ts>
struct type_array;

template <typename RemoveT, typename... Ts>
struct type_array_remove;

template <typename RemoveT>
struct type_array_remove<RemoveT> {
    using type = type_array<>;
};

template <typename RemoveT, typename First, typename... Rest>
struct type_array_remove<RemoveT, First, Rest...> {
private:
    using tail = typename type_array_remove<RemoveT, Rest...>::type;

public:
    using type = std::conditional_t<
        std::is_same_v<First, RemoveT>,
        tail,
        typename tail::template add<First>
    >;
};

template <typename... Ts>
struct type_array {
    static constexpr std::size_t size = sizeof...(Ts);

    template <std::size_t I>
    using at = std::tuple_element<I, std::tuple<Ts...>>;

    template <typename T>
    static constexpr bool has = (std::is_same_v<T, Ts> || ...);

    template <typename... Us>
    static constexpr bool has_all = (has<Us> && ...);

    template <typename NewType>
    using add = type_array<Ts..., NewType>;

    template <typename RemoveType>
    using remove = type_array_remove<RemoveType, Ts...>::type;
};

int main() {
    using A = cts<"abc"_cts>;
    using B = cts<"ab"_cts>;

    static_assert(A::equal<B::with_cts<cts<"c"_cts>>>);

    using table = ct_table<
        ct_entry<A, type_array<
            double, int
        >>
    >::add<B, double>;

    static_assert(table::get<A>::has<bool>);
    static_assert(std::is_same_v<table::get<B>, double>);
}
