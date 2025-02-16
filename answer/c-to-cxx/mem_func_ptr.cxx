#include <quiz/base.h>

class A {
private:
    int var {0};
    void(A::*cb[10])(void);

    int func0(int x) {
        var += x;
        std::cout << "calling func0"  << " var: " << var << std::endl;
        return var;
    }

    std::string func1(int x, int y) {
        var += (x + y);
        return std::to_string(var);
    }

    void register_cb() {
        cb[0] = (void(A::*)())(&A::func0);
        cb[1] = (void(A::*)())(&A::func1);
    }

public:
    void call_func0() {
        auto p  = (int(A::*)(int))cb[0];
        (this->*p)(5);
    }

    void call_func1() {
        auto p  = (std::string(A::*)(int, int))cb[1];
        auto ans = (this->*p)(5, 6);
        std::cout << "ans:" << ans << std::endl;
    }

    A() {
        register_cb();
    }
};

int main() {
    A a;
    a.call_func0();
    a.call_func1();
    return 0;
}
