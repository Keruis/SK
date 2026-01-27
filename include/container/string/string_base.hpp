#pragma once

#include "../../config/cpp_version.h"
#include "../utility/type.h"


namespace SK::Container {

template <
    #if CPP_STD >= CPP20
        Utility::Character CT, 
    #else
        typename CT, 
        typename = typename Utility::enable_if_character<CT>::type,
    #endif
    
    bool IsShared = false
>
class string_basic {
public:


private:


};

}