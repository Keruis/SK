#include <iostream>

#include "../include/container/string/string_traits.hpp"
#include "../include/config/str_optimization_level.h"


struct TestAllocator {
    using value_type = char;

    char* allocate(std::size_t n) {
        return static_cast<char*>(::operator new(n));
    }

    void deallocate(char* p, std::size_t) {
        ::operator delete(p);
    }
};

template <SK::Config::str_optimization_level SO, typename CharT, typename SizeT>
struct TestStringUtility {
    using char_t = CharT;
    static void strcpy(CharT*, const CharT*) {}
    static void strset(CharT*, CharT, std::size_t) {}
    static int  strcmp(const CharT*, const CharT*) { return 0; }
    static SizeT strlen(const CharT*) { return 0; }
};

int main() {
    using TraitsOK = SK::Container::string_traits<
        SK::Config::str_optimization_level::O1,
        char,
        TestAllocator,
        TestStringUtility,
        size_t
    >;

    TraitsOK::char_t c = 'd';
}