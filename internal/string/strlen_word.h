#ifndef STRLEN_SCALAR_H
#define STRLEN_SCALAR_H

#include <cstdint>

#include "../../foundation/types/str_types.h"
#include "pointer_utils.h"

namespace SK::Internal::String {

template <typename CT>
requires Container::Utility::Charater<CT>
[[nodiscard]] inline __attribute__((always_inline))
std::size_t strlen_scalar(const CT* str) noexcept {
    const CT* p = str;
    CT* lp = Utils::aligned_ptr(p);
    union alignas(16) U16 {
        uint64_t word[2];
    } words;
    __builtin_memcpy(&words, lp, sizeof(words));
    const std::size_t lane_off = p - lp;
    words.word[0] |= uint64_t(-1) >>
            (64 - lane_off * Utils::strlen_mask<CT>::lane_bits);
    constexpr std::size_t lanes = 8 / sizeof(CT);
    constexpr std::size_t block_lanes = 16 / sizeof(CT);
    while (true) {
        if (Utils::has_zero<CT>(words.word[0])) {
            for (std::size_t i = 0; i < lanes; ++i)
                if (lp[i] == CT(0)) return lp + i - p;
        }
        if (Utils::has_zero<CT>(words.word[1])) {
            for (std::size_t i = 0; i < lanes; ++i)
                if (lp[i + lanes] == CT(0)) return lp + i + lanes - p;
        }
        lp += block_lanes;
        __builtin_memcpy(&words, lp, sizeof(words));
    }
}

}

#endif