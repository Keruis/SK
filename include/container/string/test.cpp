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


#include <iostream>
#include <string>
#include <chrono>
#include <cstddef>
#include <new>
#include <type_traits>
#include <utility>
#include <windows.h>

/* ===================== 你的 allocator ===================== */

template <typename T>
struct simple_allocator {
    using value_type = T;

    constexpr simple_allocator() noexcept = default;

    static T* allocate(std::size_t n) {
        if (n == 0) return nullptr;
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }

    static void deallocate(T* p, std::size_t) {
        ::operator delete(p);
    }
};

/* ===================== 你的 String ===================== */
/* 你已有的头文件 */
#include "string_box.hpp"
#include "string_traits.hpp"
#include "string_core.hpp"
#include "../utility/character.h"
#include "../../../foundation/config/str_optimization_level.h"
#include "string_basic.hpp"
#include "string_context.hpp"

using MyString = SK::Container::String::string_basic<
    SK::Container::String::string_context<>,
    SK::Container::String::string_traits<
        SK::Config::str_optimization_level::Ofast,
        char,
        simple_allocator<char>,
        SK::Container::Utility::strutil
    >
>;

/* ===================== 防止优化 ===================== */

template <typename T>
inline void do_not_optimize(T const& value) {
    asm volatile("" : : "g"(value) : "memory");
}

/* ===================== 计时工具 ===================== */

using m_clock_t = std::chrono::steady_clock;

double seconds(m_clock_t::time_point start, m_clock_t::time_point end) {
    return std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
}

/* ===================== 测试参数 ===================== */

constexpr std::size_t N = 5'000'000;

/* ===================== std::string ===================== */

void test_std_string() {
    volatile std::size_t sink = 0;

    // 构造
    {
        auto t0 = m_clock_t::now();
        for (std::size_t i = 0; i < N; ++i) {
            std::string s(32, 'a');
            do_not_optimize(s);
        }
        auto t1 = m_clock_t::now();
        std::cout << "[std::string] ctor:      " << seconds(t0, t1) << " s\n";
    }

    // append
    {
        std::string s;
        auto t0 = m_clock_t::now();
        for (std::size_t i = 0; i < N; ++i) {
            s.append("abc");
            do_not_optimize(s);
        }
        auto t1 = m_clock_t::now();
        std::cout << "[std::string] append:    " << seconds(t0, t1) << " s\n";
    }

    // capacity
    {
        std::string s(128, 'x');
        auto t0 = m_clock_t::now();
        for (std::size_t i = 0; i < N; ++i) {
            sink += s.capacity();
        }
        auto t1 = m_clock_t::now();
        do_not_optimize(sink);
        std::cout << "[std::string] capacity:  " << seconds(t0, t1) << " s\n";
    }

    // resize
    {
        std::string s;
        auto t0 = m_clock_t::now();
        for (std::size_t i = 0; i < N; ++i) {
            s.resize(i % 64);
            do_not_optimize(s);
        }
        auto t1 = m_clock_t::now();
        std::cout << "[std::string] resize:    " << seconds(t0, t1) << " s\n";
    }
}

/* ===================== MyString ===================== */

void test_my_string() {
    volatile std::size_t sink = 0;

    // 构造
    {
        auto t0 = m_clock_t::now();
        for (std::size_t i = 0; i < N; ++i) {            MyString s('a', 32);
            do_not_optimize(s);
        }
        auto t1 = m_clock_t::now();
        std::cout << "[MyString]    ctor:      " << seconds(t0, t1) << " s\n";
    }

    // append
    {
        MyString s;
        auto t0 = m_clock_t::now();
        for (std::size_t i = 0; i < N; ++i) {
            s.append("abc");
            do_not_optimize(s);
        }
        auto t1 = m_clock_t::now();
        std::cout << "[MyString]    append:    " << seconds(t0, t1) << " s\n";
    }

    // capacity
    {
        MyString s('x', 128);
        auto t0 = m_clock_t::now();
        for (std::size_t i = 0; i < N; ++i) {
            sink += s.capacity();
        }
        auto t1 = m_clock_t::now();
        do_not_optimize(sink);
        std::cout << "[MyString]    capacity:  " << seconds(t0, t1) << " s\n";
    }

    // resize
    {
        MyString s;
        auto t0 = m_clock_t::now();
        for (std::size_t i = 0; i < N; ++i) {
            s.resize(i % 64);
            do_not_optimize(s);
        }
        auto t1 = m_clock_t::now();
        std::cout << "[MyString]    resize:    " << seconds(t0, t1) << " s\n";
    }
}

/* ===================== main ===================== */

int main() {
    std::cout << "sizeof(std::string) = " << sizeof(std::string) << "\n";
    std::cout << "sizeof(MyString)    = " << sizeof(MyString) << "\n\n";

    test_std_string();
    std::cout << "---------------------------------\n";
    test_my_string();
size_t r = 0;

auto stime = GetTickCount64();

{
    for (size_t i = 0; i < 1000000000; i++) {
        std::string str = "Hello";
        str.append(" World");
        r += str.size();
        do_not_optimize(str);
    }
}

auto etime = GetTickCount64();

std::cout << "Mine: " << (etime - stime) << '\n';
std::cout << r << '\n';
}


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