#include "../../foundation/types/str_types.h"
#include "../execution_policy.h"

#include "strlen_scalar.h"
#include "strcpy_scalar.h"
#include "strset_scalar.h"
#include "strcmp_scalar.h"

namespace SK::Internal::String {

template <typename CT, execution_policy>
requires Container::Utility::Character<CT>
struct string_util_basic;


template <typename CT>
requires Container::Utility::Character<CT>
struct string_util_basic<CT, execution_policy::scalar> {
    using char_t = CT;
    using pointer_t = CT*;
    using const_pointer_t = const CT*;

    static std::size_t strlen(const_pointer_t src) noexcept {
        return SK::Internal::String::strlen<char_t>(src);
    }

    static void strcpy(pointer_t dst, const_pointer_t src, std::size_t length) noexcept {
        SK::Internal::String::strncpy(dst, src, length);
    }

    static void strmove(pointer_t dst, const_pointer_t src, std::size_t length) noexcept {
        SK::Internal::String::strnmove(dst, src, length);
    }

    static void strset(pointer_t dst, char_t ch, std::size_t length) noexcept {
        SK::Internal::String::strset(dst, ch, length);
    }

    static int strcmp(const_pointer_t src1, const_pointer_t src2, std::size_t length) noexcept {
        return SK::Internal::String::strcmp(src1, src2, length);
    }
};

template <typename CT>
requires Container::Utility::Character<CT>
struct string_util_basic<CT, execution_policy::word> {
    using char_t = CT;
    using pointer_t = CT*;
    using const_pointer_t = const CT*;

    static std::size_t strlen(const_pointer_t src) noexcept {
        return SK::Internal::String::strlen<char_t>(src);
    }

    static void strcpy(pointer_t dst, const_pointer_t src, std::size_t length) noexcept {

    }

    static void strset(pointer_t dst, char_t c, std::size_t length) noexcept {

    }

    static int strcmp(const_pointer_t src1, const_pointer_t src2) noexcept {
        return 0;
    }
};

template <typename CT>
requires Container::Utility::Character<CT>
struct string_util_basic<CT, execution_policy::sse> {
    using char_t = CT;
    using pointer_t = CT*;
    using const_pointer_t = const CT*;

    static std::size_t strlen(const_pointer_t src) noexcept {
        return SK::Internal::String::strlen<char_t>(src);
    }

    static void strcpy(pointer_t dst, const_pointer_t src, std::size_t length) noexcept {

    }

    static void strset(pointer_t dst, char_t c, std::size_t length) noexcept {

    }

    static int strcmp(const_pointer_t src1, const_pointer_t src2) noexcept {
        return 0;
    }
};

template <typename CT>
requires Container::Utility::Character<CT>
struct string_util_basic<CT, execution_policy::avx> {
    using char_t = CT;
    using pointer_t = CT*;
    using const_pointer_t = const CT*;

    static std::size_t strlen(const_pointer_t src) noexcept {
        return SK::Internal::String::strlen<char_t>(src);
    }

    static void strcpy(pointer_t dst, const_pointer_t src, std::size_t length) noexcept {

    }

    static void strset(pointer_t dst, char_t c, std::size_t length) noexcept {

    }

    static int strcmp(const_pointer_t src1, const_pointer_t src2) noexcept {
        return 0;
    }
};

}