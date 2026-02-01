#include <iostream>
#include <type_traits>
#include "string_box.hpp"
#include "string_traits.hpp"
#include "../utility/character.h"
#include "../../../foundation/config/str_optimization_level.h"

int main() {
    using MyString = SK::Container::String::string_box<
        struct TestTraits
    >;

    std::cout << sizeof(MyString) << std::endl;

    std::cout << "=== Default construction ===\n";
    MyString s1;
    std::cout << "Is large? " << static_cast<int>(s1.storage.header.is_large) << "\n";
    std::cout << "Cache size: " << static_cast<int>(s1.storage.header.extent) << "\n";

    std::cout << "\n=== Cache construction (small size) ===\n";
    MyString s2(SK::Container::String::string_box<MyString::string_traits>::max_cache_size - 1);
    std::cout << "Is large? " << static_cast<int>(s2.storage.header.is_large) << "\n";
    std::cout << "Cache size: " << static_cast<int>(s2.storage.header.extent) << "\n";

    std::cout << "\n=== Large construction (big size) ===\n";
    MyString s3(SK::Container::String::string_box<MyString::string_traits>::max_cache_size + 10);
    std::cout << "Is large? " << static_cast<int>(s3.storage.header.is_large) << "\n";
    std::cout << "Large size: " << s3.storage.data.large.size << "\n";
    std::cout << "Large capacity: " << s3.storage.header.extent << "\n";

    return 0;
}