#pragma once

#include <iostream>
#include <source_location>

#ifdef DEBUG
template<typename... Ts>
static inline void print(const Ts&... args) {
    ((std::cout << args), ...);
}

template <typename... Ts>
struct debug
{
    debug(Ts&&... args, 
        const std::source_location& loc = std::source_location::current())
    {
        print(loc.function_name(), ":", loc.line());
        print(" [", args..., "]\n");
    }
};

template <typename... Ts>
debug(Ts&&...args) -> debug<Ts...>;
#else
template<typename... Ts>
void debug(Ts&&... args) {}
#endif
