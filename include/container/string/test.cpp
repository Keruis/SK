#include <iostream>
#include <type_traits>
#include "string_box.hpp"
#include "string_traits.hpp"
#include "../utility/character.h"
#include "../../../foundation/config/str_optimization_level.h"


struct TestAllocator {
    using value_type = char;

    char* allocate(std::size_t n) {
        return static_cast<char*>(::operator new(n));
    }

    void deallocate(char* p, std::size_t) {
        ::operator delete(p);
    }
};


int main() {
    using MyString = SK::Container::String::string_box<
            SK::Container::string_traits<
                SK::Config::str_optimization_level::O1,
                char,
                TestAllocator,
                SK::Container::Utility::strutil
        >
    >;

    MyString s;
    std::cout << "string_box constructed.\n";
    std::cout << "size: " << sizeof(s) << std::endl;

    std::cout << "Cache size: " << static_cast<int>(s.storage.cache.size) << "\n";
    std::cout << "Cache is_large: " << static_cast<int>(s.storage.cache.is_large) << "\n";

    s.storage.cache.data[0] = 'H';
    s.storage.cache.data[1] = 'i';
    s.storage.cache.size = 2;
    s.storage.cache.is_large = 0;

    std::cout << "Cache data: ";
    for (std::size_t i = 0; i < s.storage.cache.size; ++i) {
        std::cout << s.storage.cache.data[i];
    }
    std::cout << "\n";

    s.storage.large.data = new char[100];
    s.storage.large.size = 100;
    s.storage.large.capacity = 127;
    s.storage.large.is_large = 1;

    std::cout << "Large size: " << s.storage.large.size << "\n";
    std::cout << "Large capacity: " << s.storage.large.capacity << "\n";
    std::cout << "Large is_large: " << s.storage.large.is_large << "\n";

    delete[] s.storage.large.data;

    return 0;
}