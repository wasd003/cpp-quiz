#include <quiz/base.h>

template<typename F, typename... Args>
struct invoke_result {
    using type = decltype(std::declval<F>()(std::declval<Args>()...));
};

struct S {
    char operator()(char, char) { return 'a'; };
    int operator()(int, int) { return 1; }
};

template<typename T>
void dump_type() {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}

void test_result_of() {
    using R1 = typename std::result_of<S(int, int)>::type;
    using R2 = typename std::result_of<S(char, char)>::type;
    static_assert(std::is_same_v<R1, int>);
    static_assert(std::is_same_v<R2, char>);
}

void test_invoke_result() {
    using R1 = typename invoke_result<S, int, int>::type;
    using R2 = typename invoke_result<S, char, char>::type;
    static_assert(std::is_same_v<R1, int>);
    static_assert(std::is_same_v<R2, char>);
}

int main() {
    test_result_of();

    test_invoke_result();
    return 0;
}
