#include <iostream>
#include <quiz/base.h>
#include <string_view>

void func(std::string_view& sv) {
    std::cout << sv << std::endl;
}

int main() {
    std::string_view sv("abc");
    auto sub = sv.substr(1, 1);
    func(sub);
    return 0;
}
