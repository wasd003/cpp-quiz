#include <quiz/base.h>
#include <quiz/log.h>

template<typename Last>
int recursive_add(Last last) {
    return last;
}

template<typename First, typename... Rest>
int recursive_add(First first, Rest... rest) {
    return first + recursive_add(rest...);
}

template<typename First, typename... Rest>
int fold_add(First first, Rest... rest) {
    return (first + ... + rest);
}

void printk(const std::string_view &s) {
    std::cout << s;
}

template<typename First, typename... Rest>
void printk(const std::string_view &s, const First& first, const Rest&... rest) {
    if (s.empty()) return;
    if (s.size() >= 2 && s[0] == '{' && s[1] == '}') [[unlikely]] {
        std::cout << first;
        printk(s.substr(2), rest...);
    } else {
        std::cout << s[0];
        printk(s.substr(1), first, rest...);
    }
}

void test_add() {
    auto ans1 = recursive_add(1, 2, 3);
    auto ans2 = fold_add(1, 2, 3);
    assert(ans1 == ans2);
}

void test_print() {
    printk("my age is:{} my height is:{}\n", 18, 180);

    printk("my name is:{} my weight is:{}. I say: hello world\n", "jack", 70);
}

int main() {

    test_add();

    test_print();
}
