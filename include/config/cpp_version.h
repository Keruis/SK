#pragma once

#define CPP_STD __cplusplus

#define CPP11 201103L
#define CPP14 201402L
#define CPP17 201703L
#define CPP20 202002L
#define CPP23 202302L

#if CPP_STD >= CPP23
    #define IF_CPP23(...) __VA_ARGS__
#else
    #define IF_CPP23(...)
#endif

#if CPP_STD >= CPP20
    #define IF_CPP20(...) __VA_ARGS__
#else
    #define IF_CPP20(...)
#endif

#if CPP_STD >= CPP17
    #define IF_CPP17(...) __VA_ARGS__
#else
    #define IF_CPP17(...)
#endif

#if CPP_STD >= CPP14
    #define IF_CPP14(...) __VA_ARGS__
#else
    #define IF_CPP14(...)
#endif

#if CPP_STD >= CPP11
    #define IF_CPP11(...) __VA_ARGS__
#else
    #define IF_CPP11(...)
#endif