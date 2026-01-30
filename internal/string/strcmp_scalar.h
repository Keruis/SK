#pragma once

#include "../../foundation/types/str_types.h"

namespace SK::Internal::String {

template <typename CT>
requires Container::Utility::Character<CT>
int strcmp(const CT* src1, const CT* src2, std::size_t n) noexcept {
    while (n-- && (*src1 == *src2)) {
        ++src1;
        ++src2;
    }
    return static_cast<int>(*src1) - static_cast<int>(*src2);
}

}