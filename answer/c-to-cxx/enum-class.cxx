#include <quiz/base.h>
#include <quiz/log.h>

enum class color : uint64_t {
    RED,
    GREEN,
    BLUE,
};

decltype(auto) get(const color& c) {
    return static_cast<std::underlying_type_t<color>> (c);
}

int main() {
    debug(get(color::RED));
}