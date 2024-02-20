#include <functional>
#include <quiz/base.h>
#include <quiz/log.h>

template<typename T>
struct base {
    void func() {
        static_cast<T*>(this)->func_impl();
    }
};

struct derived1 : base<derived1> {
    void func_impl() {
        debug("");
    }
};

struct derived2 : base<derived2> {
    void func_impl() {
        debug("");
    }
};

void T1() {
    derived1 d1;
    derived2 d2;
    d1.func();
    d2.func();
}

template<typename T>
struct base_op {
    bool operator>(const T& rhs) const {
        return (!static_cast<const T*>(this)->operator<(rhs)) &&
            (!static_cast<const T*>(this)->operator==(rhs));
    }
};

struct integer: base_op<integer>  {
    int val;
    integer(int val) : val(val) {}

    bool operator<(const integer& rhs) const {
        return val < rhs.val;
    }

    bool operator==(const integer& rhs) const {
        return val == rhs.val;
    }
};

void T2() {
    integer a {1}, b {2};
    auto ans1 = a > b;
    auto ans2 = a > a;
    std::cout << ans1 << " " << ans2;
}

int main() {
    T1();

    T2();
}
