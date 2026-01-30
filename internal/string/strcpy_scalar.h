#pragma once

#include "../../foundation/types/str_types.h"

namespace SK::Internal::String {

template <typename CT>
requires Container::Utility::Character<CT>
constexpr void strncpy(CT* dst, const CT* src, std::size_t n) noexcept {
    while (n--) {
        *dst++ = *src++;
    }
}

template <typename CT>
requires Container::Utility::Character<CT>
constexpr void strnmove(CT* dst, const CT* src, std::size_t n) noexcept {
    if (dst == src || n == 0) return;

    const CT* src_end = src + n;
    if (src < dst && src_end > dst) {
        for (CT* p = dst + n - 1; p >= dst; --p) {
            *p = *(--src_end);
        }
    } else {
        strncpy(dst, src, n);
    }
}

}