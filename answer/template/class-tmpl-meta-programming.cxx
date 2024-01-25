#include <iterator>
#include <quiz/base.h>
#include <quiz/log.h>
#include <type_traits>

template<typename T, typename = void>
struct t1 {
    constexpr static int val = 0;
};

template<typename T>
struct t1<T, std::enable_if_t<std::is_integral_v<T>>> {
    constexpr static int val = 1;
};

template<typename T>
struct t1_cpt {
    constexpr static int val = 0;
};

template<typename T>
requires std::is_integral_v<T>
struct t1_cpt<T> {
    constexpr static int val = 1;
};

void T1() {
    static_assert(t1<int>::val == 1);
    static_assert(t1<float>::val == 0);
    static_assert(t1_cpt<int>::val == 1);
    static_assert(t1_cpt<float>::val == 0);
}

template<typename F, typename... Ts>
struct t2_cpt {
    constexpr static int val = 0;
};

template<typename F, typename... Ts>
requires requires {
    requires std::is_integral_v<std::result_of_t<F(Ts...)>>;
}
struct t2_cpt<F, Ts...> {
    constexpr static int val = 1;
};

void T2() {
    auto f1 = [](int, int) { return 42; };
    auto f2 = [](int, int) {};
    static_assert(t2_cpt<decltype(f1), int, int>::val == 1);
    static_assert(t2_cpt<decltype(f2), int, int>::val == 0);
}

template<typename T, typename = void>
struct t3 {
    constexpr static int val = 0;
};

template<typename T>
struct t3<T, std::void_t<typename T::nested_type>> {
    constexpr static int val = 1;
};

template<typename T, typename = void>
struct t3_cpt {
    constexpr static int val = 0;
};

template<typename T>
requires requires {
    typename T::nested_type;
}
struct t3_cpt<T> {
    constexpr static int val = 1;
};

void T3() {
    struct has_nested_type { using nested_type = int; };
    static_assert(t3<has_nested_type>::val == 1);
    static_assert(t3<int>::val == 0);
    static_assert(t3_cpt<has_nested_type>::val == 1);
    static_assert(t3_cpt<int>::val == 0);
}

int main() {
    T1();
    T2();
    T3();
}
