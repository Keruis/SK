#pragma once

#include "../../../foundation/types/str_types.h"
#include "../../../foundation/config/str_optimization_level.h"

#include "string_core.hpp"

namespace SK::Container::String {

template <
    typename BSCTX,
    typename ST
> requires Utility::StringTraits<ST>
class string_basic : public string_core<BSCTX, ST> {
    using core_t = string_core<BSCTX, ST>;
    
public:
    using core_t::core_t;

    template <typename SelfType, typename... Args>
    constexpr decltype(auto) append(this SelfType&& self, Args&&... args) 
        noexcept (
            core_t::string_context::template has_v<string_flag::Noexcept>
        ) requires (
            requires {
                (self.append_impl(std::forward<Args>(args)), ...);
            } && 
            !core_t::string_context
                ::template has_v<string_flag::Mutable>
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