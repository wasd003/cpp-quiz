#include <quiz/base.h>
#include <quiz/log.h>

struct test {
    int a, b, c;
    constexpr test(int a, int b, int c) : a(a), b(b), c(c) {}
};

struct widget {
    // const propgation
    static const int N = 100;
    static const int M;
    static const std::string const_string;
    static std::string non_const_string;

    // inline static variable
    inline static std::string str = "I am inline static variable";

    // constexpr data is implict inline since c++17
    constexpr static test test_obj {1, 2, 3};
};

const std::string widget::const_string = "const-string";
std::string widget::non_const_string = "non-const-string";
const int widget::M = 200;

int main() {
    auto x = widget::N;
    debug(x, " ", widget::M, " ", widget::const_string, " ", widget::non_const_string);
    auto& sr = widget::str;
    std::cout << sr << std::endl;
    debug(widget::test_obj.a);
    return 0;
}
