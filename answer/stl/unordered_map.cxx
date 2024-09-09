#include <functional>
#include <quiz/base.h>
#include <unordered_map>

struct test {
    int a, b, c;
    bool operator==(const test& rhs) const {
        return a == rhs.a && b == rhs.b && c == rhs.c;
    }
};

template<>
struct std::hash<test> {
    size_t operator()(const test& t) const {
        return std::hash<int>{}(t.a) ^ std::hash<int>{}(t.b) ^ std::hash<int>{}(t.c);
    }
};

int main() {
    std::unordered_map<test, int> h;
    h[{1, 2, 3}] = 4;
    std::cout << h[{1, 2, 3}];
    return 0;
}
