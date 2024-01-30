#include <quiz/base.h>
#include <quiz/log.h>

struct base {
    virtual ~base() {}
};

struct derived : public base {
    virtual ~derived() {}
};
void sc() {
    // bultin type cast
    double x = 1;
    int y = static_cast<int> (x);

    // derived class cast
    derived dobj;
    base bobj;
    derived& d = dobj;
    base& b = bobj;
    auto& bd = static_cast<derived&>(b); // Danger!
    auto& db = static_cast<base&>(d);

    UNUSED(y); UNUSED(bd); UNUSED(db);
}

void rc() {
    // integral <-> pointer
    void *ptr = nullptr;
    uint64_t val = reinterpret_cast<uint64_t>(ptr);

    // pointer <-> pointer
    uint64_t *up = reinterpret_cast<uint64_t *>(ptr);

    // do not use reinterpret cast to cast between number !
    // double x = 3.1415926;
    // uint64_t y = reinterpret_cast<uint64_t> (x);

    UNUSED(val); UNUSED(up);
}

void dc() {
    base* b = new base();
    auto* bd = dynamic_cast<derived*>(b);
    assert(!bd);
    delete b;
}

void cc() {
    const volatile base *b = new base();
    auto *y = const_cast<base *>(b);
    UNUSED(y);
    delete b;
}


int main() {
    sc();
    rc();
    dc();
    cc();
}
