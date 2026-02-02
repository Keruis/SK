#pragma once

#include "../../../foundation/types/str_types.h"
#include "../../../foundation/config/str_optimization_level.h"
#include "../../../internal/string/string_util_basic.h"
#include "../../../internal/execution_policy.h"

namespace SK::Container::Utility {

template <
    Config::str_optimization_level SO, 
    typename CT
> struct strutil;

template <
    typename CT
> requires Character<CT>
struct strutil<
    Config::str_optimization_level::O0, 
    CT
> : Internal::String::string_util_basic<
        CT, 
        Internal::execution_policy::scalar
    > {};

template <
    typename CT
> requires Character<CT>
struct strutil<
    Config::str_optimization_level::Ofast, 
    CT
> : Internal::String::string_util_basic<
        CT,
        Internal::execution_policy::scalar
    > {};

    
}