#ifndef STRSET_SCALAR_H
#define STRSET_SCALAR_H

#include <cstdint>
#include <iostream>

#include "pointer_utils.h"

namespace SK::Internal::String {

    template <typename CT>
    inline __attribute__((always_inline))
    void strset_scalar(CT* dest, CT c, uint64_t length) noexcept {
        CT* p = dest;

        constexpr std::size_t min_bulk_chars = (4 * sizeof(uint64_t)) / sizeof(CT);

        if (length < min_bulk_chars || !Utils::is_properly_aligned(p)) 
            goto tail_only;

        if (!Utils::is_aligned(p)) {
            std::size_t align_fill = Utils::chars_to_align(dest);
            length -= align_fill;
            while (align_fill--) *p++ = c;
        }

        {
            using mask_t = typename string_mask<CT>::mask_t;
            constexpr std::size_t chars_per_word = sizeof(mask_t) / sizeof(CT);
            constexpr std::size_t chars_per_block = chars_per_word * 2;

            mask_t word = string_mask<CT>::broadcast(c);
            union alignas(16) U16 {
                mask_t word[2];
            } words {word, word};
            
            std::size_t num_blocks = length / chars_per_block;
            length %= chars_per_block;
            while (num_blocks--) {
                __builtin_memcpy(p, &words, sizeof(words));
                p += chars_per_block;
            }
            
            std::size_t num_words = length / chars_per_word;
            length %= chars_per_word;
            while (num_words--) {
                __builtin_memcpy(p, &word, sizeof(word));
                p += chars_per_word;
            }
        }

        tail_only:
            while (length--) *p++ = c;        
    }
}


#endif //STRSET_SCALAR_H
