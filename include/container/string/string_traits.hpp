#pragma once

#include "../../../foundation/config/str_optimization_level.h"
#include "../../../foundation/types/str_types.h"

namespace SK::Container::String {

template <
    Config::str_optimization_level SO,
    typename CT,
    typename AT,
    template <Config::str_optimization_level, typename> typename SU
> requires (
    Utility::StringUtility<SU<SO, CT>> &&
    Utility::Character<CT> &&
    Utility::Allocator<AT, CT>
) struct string_traits {
    using char_t            =             CT;
    using reference_t       =            CT&;
    using const_reference_t =      const CT&;
    using pointer_t         =            CT*;
    using const_pointer_t   =      const CT*;

    using alloc_t           =             AT;
    using strutil           =     SU<SO, CT>;

    static constexpr Config::str_optimization_level str_optimization = SO;
};

}