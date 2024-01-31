#include <quiz/base.h>
#include <quiz/log.h>

struct widget {
    int val {42};
    constexpr widget() {}
};

struct wrapper {
    constinit static widget inclass_static_var;
};
constinit widget wrapper::inclass_static_var;

constinit widget global_var;

void func() {
    constinit static widget local_static_var;
    debug(local_static_var.val);
}

int main() {
    debug(global_var.val);
    func();
    debug(wrapper::inclass_static_var.val);
}
