#include <quiz/base.h>
#include <quiz/log.h>

struct widget {
    // const propgation
    static const int N = 100;
    static const int M;
    static const std::string const_string;
    static std::string non_const_string;
};

const std::string widget::const_string = "const-string";
std::string widget::non_const_string = "non-const-string";
const int widget::M = 200;

int main() {
    auto x = widget::N;
    debug(x, " ", widget::M, " ", widget::const_string, " ", widget::non_const_string);
    return 0;
}