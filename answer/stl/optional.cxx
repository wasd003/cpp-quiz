#include <quiz/base.h>

struct nullopt_t {};

nullopt_t nullopt;

template<typename T>
struct optional {
private:
    bool valid;
    T data;

public:
    optional(const T& data) : valid(true), data(data) {}

    optional(T&& data) : valid(true), data(std::move(data)) {}

    optional(const nullopt_t&) : valid(false) {}

    operator bool() {
        return valid;
    }

    T* operator->() {
        return &data;
    }

    T& operator*() {
        return data;
    }
};

optional<int> func() {
    return nullopt;
}

int main() {
    auto ans = func();
    assert(!ans);
    return 0;
}
