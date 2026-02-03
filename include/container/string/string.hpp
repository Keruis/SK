#pragma once

#include "../../../foundation/types/str_types.h"
#include "../../../foundation/config/str_optimization_level.h"

#include "string_core.hpp"

namespace SK::Container::String {

template <
    typename ST
> requires Utility::StringTraits<ST>
class string : public string_core<ST> {
    using core_t = string_core<ST>;
public:
    using core_t::core_t;

    template <typename SelfType, typename... Args>
    constexpr decltype(auto) append(this SelfType&& self, Args&&... args) 
        noexcept requires (
            requires {
                (self.append_impl(std::forward<Args>(args)), ...);
            }
        )
    {
        return (
            (
                self.append_impl(std::forward<Args>(args)),
                ...
            ), 
            std::forward<SelfType>(self)
        );
    }

private:

};
    
}