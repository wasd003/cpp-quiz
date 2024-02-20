#include <quiz/base.h>

template<typename F, typename... Args>
struct mini_result_of {
    using type = decltype( std::declval<F>()(std::declval<Args>()... ));
};

int add(int a, int b) {
    return a + b;
}

int main() {
    using type = typename mini_result_of<decltype(add), int, int>::type;
    static_assert(std::is_same_v<int, type>);
}
