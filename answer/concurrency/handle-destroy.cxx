#include <future>
#include <quiz/base.h>

int main() {
    auto future = std::async([]() {
        for (;;) {
            std::cout << "running" << std::endl;
        }
    });
    return 0;
}
