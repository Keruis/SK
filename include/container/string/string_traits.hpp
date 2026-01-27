#pragma once

#include "../../config/cpp_version.h"
#include "../utility/type.h"

namespace SK::Container {

template <
    #if CPP_STD >= CPP20
        Utility::Character CT,
        Utility::Allocator AT,
        template <Utility::Character, typename> typename SU,
        Utility::SizeType ST
    #else
        typename CT,
        typename AT,
        template <typename, typename> typename SU,
        typename ST,
        typename enable = void
    > struct string_traits {
        static_assert(
            Utility::is_character_type<CT>(),
            "string_traits: CT must be a character type"
        ); static_assert(
            Utility::is_allocator_type<AT, ST>::value,
            "string_traits: AT must be a valid allocator type for size type ST"
        ); static_assert(
            Utility::is_string_utility<SU<CT, ST>, ST>::value,
            "string_traits: StringUtility<CT, ST> must satisfy string utility requirements"
        ); static_assert(
            Utility::is_size_type<ST>::value,
            "string_traits: ST must be an integral type (size type)"
        );
    };
    template<
        typename CT,
        typename AT,
        template <typename, typename> typename SU,
        typename ST
    > struct string_traits<
        CT, AT, SU, ST,
        typename std::enable_if<
            Utility::is_character_type<CT>() &&
            Utility::is_allocator_type<AT, ST>::value &&
            Utility::is_string_utility<SU<CT, ST>, ST>::value &&
            Utility::is_size_type<ST>::value
        >::type
    #endif
> IF_CPP20 (
    requires (
        Utility::is_character_type<typename AT::value_type> &&
        Utility::StringUtility<SU<CT, ST>>
    )
) IF_CPP20(struct string_traits) {
    using char_t            =           CT;
    using reference_t       =          CT&;
    using pointer_t         =          CT*;
    using const_pointer_t   =    const CT*;
};

}