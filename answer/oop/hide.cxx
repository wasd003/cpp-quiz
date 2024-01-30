#include <quiz/base.h>

struct base {
    int add(int a, int b) { return a + b; }
};

struct derived : public base {
    int add(int a) { return a; }
    using base::add;
    void func() {
        add(1, 2);
        add(1);
    }
};
int main() {

}