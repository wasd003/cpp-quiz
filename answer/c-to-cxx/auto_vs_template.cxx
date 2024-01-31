#include <quiz/base.h>
#include <quiz/log.h>

template<typename T>
void func(const T&) {
}

int main() {
    // ERROR for template type deduction
    // func({1, 2, 3});

    auto list = {1, 2, 3};
    UNUSED(list);

    return 0;
}
