#include <quiz/base.h>
#include <quiz/log.h>

void t2() {
    // 如果同时存在则member initializer list优先级更高
    struct widget {
        widget(int x) { assert(x); }
    };
    struct demo {
        widget w {0};
        demo() : w(1) {}
    };
    demo d;
}

void t3() {
    struct widget_without_ilist {
        int a, b, c;
        widget_without_ilist(int a, int b, int c) : a(a), b(b), c(c) {  debug("#1"); }
    };
    struct widget_with_ilist {
        int a, b, c;
        widget_with_ilist(int a, int b, int c) : a(a), b(b), c(c) { debug("#2"); }
        widget_with_ilist(const std::initializer_list<int>& list) { debug("#3"); }
        widget_with_ilist() { debug("#4"); }
    };
    // case#1
    widget_without_ilist x1 = {1, 2, 3};
    // case#2
    widget_with_ilist x2 = {1, 2, 3};
    // case#3
    widget_with_ilist x3 = {};
    widget_with_ilist x4 = {{}};

    UNUSED(x1); UNUSED(x2);
    UNUSED(x3); UNUSED(x4);
}

int main() {
    t2();

    t3();
}