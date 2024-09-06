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

void printk(const std::string_view& fmt) {
    std::cout << fmt;
}

template<typename First, typename... Rest>
void printk(const std::string_view& fmt, const First& first, const Rest&... rest) {
    const int n = fmt.size();
    for (int i = 0; i < n; i ++ ) {
        if (fmt[i] == '{' && i + 1 < n && fmt[i + 1] == '}') {
            std::cout << first;
            printk(fmt.substr(i + 2), rest...);
            return;
        } else {
            std::cout << fmt[i];
        }
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
