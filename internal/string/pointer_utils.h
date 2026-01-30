#pragma once

#include <cstdint>

#include "../../foundation/types/str_types.h"

namespace SK::Internal::String::Utils {

template <typename CT>
requires Container::Utility::Charater<CT>
struct string_mask {
    using mask_t = uint64_t;

    static constexpr mask_t lane_bits = sizeof(CT) * 8;

    static constexpr mask_t kMask01 =
        mask_t(-1) / ((mask_t(1) << lane_bits) - 1);

    static constexpr mask_t kMask80 =
        kMask01 << (lane_bits - 1);

    static constexpr const mask_t broadcast(CT ch) noexcept {
        return static_cast<uint64_t>(ch) * kMask01;
    }
};

template <typename CT>
requires Container::Utility::Charater<CT>
constexpr inline CT* aligned_ptr(const CT* p) noexcept {
    return reinterpret_cast<CT*>(
        reinterpret_cast<uintptr_t>(p) & ~(sizeof(uint64_t)-1)
    );
}

template <typename CT>
requires Container::Utility::Charater<CT>
constexpr inline bool has_zero(uint64_t word) noexcept {
    constexpr uint64_t mask01 = strlen_mask<CT>::kMask01;
    constexpr uint64_t mask80 = strlen_mask<CT>::kMask80;
    return (word - mask01) & (~word & mask80);
}

template <typename CT>
requires Container::Utility::Charater<CT>
constexpr inline bool is_aligned(const CT* p) noexcept {
    return (reinterpret_cast<uintptr_t>(p) & (sizeof(uint64_t) - 1)) == 0;
}

template <typename CT>
requires Container::Utility::Charater<CT>
constexpr inline size_t chars_to_align(const CT* p) noexcept {
    constexpr size_t word_bytes = sizeof(uint64_t);
    uintptr_t addr = reinterpret_cast<uintptr_t>(p);
    size_t mis = addr & (word_bytes - 1);
    if (mis == 0) return 0;

    size_t bytes = word_bytes - mis;
    return (bytes + sizeof(CT) - 1) / sizeof(CT);
}

template <typename CT>
requires Container::Utility::Charater<CT>
constexpr inline bool is_properly_aligned(const CT* p) noexcept {
    uintptr_t addr = reinterpret_cast<uintptr_t>(p);
    return (addr % sizeof(CT)) == 0;
}

}