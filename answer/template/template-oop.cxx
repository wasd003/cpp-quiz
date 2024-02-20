#include <quiz/base.h>
#include <quiz/log.h>

template<typename T>
struct Base {
    void func() { debug(""); }
};

template<typename T>
struct Derived : public Base<T> {
    using Base<T>::func;

    void derived_func() {
        this->func();
        Base<T>::func();
        func();
    }
};

int main() {
    Derived<int> {}.derived_func();
}
