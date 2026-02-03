#include <iostream>
#include <type_traits>
#include "string_box.hpp"
#include "string_traits.hpp"
#include "string_core.hpp"
#include "../utility/character.h"
#include "../../../foundation/config/str_optimization_level.h"
#include <cstddef>
#include <new>
#include <string>
#include "string.hpp"

template <typename T>
struct simple_allocator {
    using value_type = T;

    constexpr simple_allocator() noexcept = default;

    static constexpr T* allocate(std::size_t n) {
        if (n == 0) return nullptr;
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }

    static constexpr void deallocate(T* p, std::size_t) {
        ::operator delete(p);
    }
};

int main() {
    using MyString = SK::Container::String::string<
        SK::Container::String::string_traits<
            SK::Config::str_optimization_level::O0,
            char,
            simple_allocator<char>,
            SK::Container::Utility::strutil
        >
    >;

    std::cout << "String size = " << sizeof(MyString) << std::endl;

    MyString s1('a', 5);
    std::cout << s1.data() << std::endl;
    std::cout << s1.size() << std::endl;
    std::cout << s1.capacity() <<std::endl;

    s1.append('b', "abc", MyString{}.append('r'));
    std::cout << s1.data() << std::endl;
    std::cout << s1.size() << std::endl;
    std::cout << s1.capacity() <<std::endl;
    return 0;
}