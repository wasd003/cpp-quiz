#include <quiz/base.h>

template<typename T>
decltype(auto) mini_move(T&& val) noexcept {
    return static_cast<std::remove_reference_t<T>&&>(val);
}

template<typename T>
decltype(auto) mini_forward(std::remove_reference_t<T>& val) noexcept {
    return static_cast<T&&>(val);
}

template<typename T>
decltype(auto) mini_forward(std::remove_reference_t<T>&& val) noexcept {
    return static_cast<T&&>(val);
}

int main() {
    return 0;
}
