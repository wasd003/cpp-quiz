#include <quiz/base.h>
#include <quiz/log.h>
#include <type_traits>

int main() {
    int x = 1;
    static_assert(std::is_same_v<int&, decltype((x))>);
    UNUSED(x);
    return 0;
}
