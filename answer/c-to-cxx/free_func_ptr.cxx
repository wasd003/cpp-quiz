#include <quiz/base.h>


void (*callbacks[10])(void);

int add0(int a, int b) {
    return a + b;
}

int add1(int a, int b, int c) {
    return a + b + c;
}

void register_cb() {
    callbacks[0] = (void(*)())add0;
    callbacks[1] = (void(*)())add1;
}

void call_add0() {
    auto f = (int(*)(int, int))callbacks[0];
    auto ans = f(1, 2);
    std::cout << ans << std::endl;
}

void call_add1() {
    auto f = (int(*)(int, int, int))callbacks[1];
    auto ans = f(1, 2, 5);
    std::cout << ans << std::endl;
}

int main() {
    register_cb();
    call_add0();
    call_add1();
    return 0;
}
