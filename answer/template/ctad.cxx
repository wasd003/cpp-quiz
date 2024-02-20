#include <quiz/base.h>
#include <source_location>

template<typename... Args>
struct auto_log {
    auto_log(Args... args, std::source_location loc = std::source_location::current()) {
        std::cout << loc.function_name() << ":" << loc.line() << ":";
        ((std::cout << args), ...);
    }
};

template<typename... Args>
auto_log(Args...) -> auto_log<Args...>;

int main() {
    auto_log();
    auto_log("as", ":", 12);
}
