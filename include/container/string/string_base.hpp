#pragma once

#include "../../../foundation/config/cpp_version.h"
#include "../../../foundation/types/str_types.h"


namespace SK::Container {

template <
    typename CT, 
    bool IsShared = false
> requires Utility::Character<CT>
class string_basic {
public:
    string_basic();
    ~string_basic();

private:


};

#include "string_base.hxx"

}