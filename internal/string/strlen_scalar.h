#pragma once

#include "../../foundation/types/str_types.h"

namespace SK::Internal::String {

template <typename CT>
requires Container::Utility::Character<CT>
[[nodiscard]] constexpr std::size_t strlen(const CT* src) noexcept {
    const CT* p = src;
    while (*p) p++;
    constexpr std::size_t char_size = sizeof(CT);
    return static_cast<std::size_t>((p - src) / char_size);
}

}