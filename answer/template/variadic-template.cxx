#include <quiz/base.h>
#include <quiz/log.h>

template<typename Last>
int recursive_add(Last last) {
    return last;
}

template<typename First, typename... Rest>
int recursive_add(First first, Rest... rest) {
    return first + recursive_add(rest...);
}

template<typename First, typename... Rest>
int fold_add(First first, Rest... rest) {
    return (first + ... + rest);
}

int main() {
    auto ans1 = recursive_add(1, 2, 3);
    auto ans2 = fold_add(1, 2, 3);
    assert(ans1 == ans2);
}
