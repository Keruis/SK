#pragma once

#include "../../config/cpp_version.h"
#include "../../config/str_optimization_level.h"
#include "../utility/type.h"

namespace SK::Container {

template <
    Config::str_optimization_level SO,
    #if CPP_STD >= CPP20
        Utility::Character CT,
        Utility::Allocator<CT> AT,
        template <Config::str_optimization_level, Utility::Character, typename> typename SU,
        Utility::SizeType ST = std::size_t
    #else
        typename CT,
        typename AT,
        template <Config::str_optimization_level, typename, typename> typename SU,
        typename ST = std::size_t
    #endif
> IF_CPP20 (
    requires (
        Utility::StringUtility<SU<SO, CT, ST>>
    )
) struct string_traits {
#if CPP_STD < CPP20
    static_assert(
        Utility::is_character_type<CT>(),
        "string_traits: CT must be a character type"
    ); static_assert(
        Utility::is_allocator_type<AT, ST>(),
        "string_traits: AT must be a valid allocator type for size type ST"
    ); static_assert(
        Utility::is_string_util<SU<SO, CT, ST>, ST>(),
        "string_traits: StringUtility<CT, ST> must satisfy string utility requirements"
    ); static_assert(
        Utility::is_size_type<ST>(),
        "string_traits: ST must be an integral type (size type)"
    ); static_assert(
        Utility::is_character_type<typename AT::value_type>(),
        "string_traits: AT::value_type must be a character type"
    );
#endif
    using size_t            =             ST;

    using char_t            =             CT;
    using reference_t       =            CT&;
    using const_reference_t =      const CT&;
    using pointer_t         =            CT*;
    using const_pointer_t   =      const CT*;

    using alloc_t           =             AT;
    using strutil           = SU<SO, CT, ST>;

    constexpr static Config::str_optimization_level str_optimization = SO;
};

}