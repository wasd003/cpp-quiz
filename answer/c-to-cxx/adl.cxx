#include <quiz/base.h>
#include <quiz/log.h>

namespace cc {
    struct widget {};
    void func(const widget&) {
        debug("");
    }
};

int main() {
    func(cc::widget{});
}
