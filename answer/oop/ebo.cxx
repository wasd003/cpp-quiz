#include <quiz/base.h>
struct empty {};

struct a : private empty {
    int x;
};

struct b {
    empty e;
    int x;
};

struct c {
    [[no_unique_address]] empty e;
    int x;
};

int main() {
    static_assert(sizeof (a) == 4);
    static_assert(sizeof (b) == 8);
    static_assert(sizeof (c) == 4);
}