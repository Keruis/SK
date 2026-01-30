#pragma once

#include "../../foundation/types/str_types.h"

namespace SK::Internal::String {

template <typename CT>
requires Container::Utility::Character<CT>
constexpr void strset(CT* dst, CT ch, std::size_t n) noexcept {
    while (n--) {
        *dst++ = ch;
    }
}

}