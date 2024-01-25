#include <quiz/base.h>

void non_const_lvalue_ref() {
    int x = 27;
    const int cx = x;
    const int& rx = x;

    {
        auto f = []<typename T>(T& param) {
            static_assert(std::is_same_v<T, int>);
            static_assert(std::is_same_v<decltype(param), int&>);
        };
        f(x);
    }
    {
        auto f = []<typename T>(T& param) {
            static_assert(std::is_same_v<T, const int>);
            static_assert(std::is_same_v<decltype(param), const int&>);
        };
        f(cx);
    }
    {
        auto f = []<typename T>(T& param) {
            static_assert(std::is_same_v<T, const int>);
            static_assert(std::is_same_v<decltype(param), const int&>);
        };
        f(rx);
    }
}

void const_lvalue_ref() {
    int x = 27;
    const int cx = x;
    const int& rx = x;

    {
        auto f = []<typename T>(const T& param) {
            static_assert(std::is_same_v<T, int>);
            static_assert(std::is_same_v<decltype(param), const int&>);
        };
        f(x);
    }
    {
        auto f = []<typename T>(const T& param) {
            static_assert(std::is_same_v<T, int>);
            static_assert(std::is_same_v<decltype(param), const int&>);
        };
        f(cx);
    }
    {
        auto f = []<typename T>(const T& param) {
            static_assert(std::is_same_v<T, int>);
            static_assert(std::is_same_v<decltype(param), const int&>);
        };
        f(rx);
    }
}

void universal_ref() {
    int x = 27;
    const int cx = x;
    const int& rx = x;

    {
        auto f = []<typename T>(T&& param) {
            static_assert(std::is_same_v<T, int&>);
            static_assert(std::is_same_v<decltype(param), int&>);
        };
        f(x);
    }
    {
        auto f = []<typename T>(T&& param) {
            static_assert(std::is_same_v<T, const int&>);
            static_assert(std::is_same_v<decltype(param), const int&>);
        };
        f(cx);
    }
    {
        auto f = []<typename T>(T&& param) {
            static_assert(std::is_same_v<T, const int&>);
            static_assert(std::is_same_v<decltype(param), const int&>);
        };
        f(rx);
    }
}

void value() {
	int x = 27;           
	const int cx = x;     
	const int &rx = cx;  
	const char* const ptr = "ABC";

    {
        auto f = []<typename T>(T param) {
            static_assert(std::is_same_v<T, int>);
            static_assert(std::is_same_v<decltype(param), int>);
        };
        f(x);
    }
    {
        auto f = []<typename T>(T param) {
            static_assert(std::is_same_v<T, int>);
            static_assert(std::is_same_v<decltype(param), int>);
        };
        f(cx);
    }
    {
        auto f = []<typename T>(T param) {
            static_assert(std::is_same_v<T, int>);
            static_assert(std::is_same_v<decltype(param), int>);
        };
        f(rx);
    }
    {
        auto f = []<typename T>(T param) {
            static_assert(std::is_same_v<T, const char*>);
            static_assert(std::is_same_v<decltype(param), const char*>);
        };
        f(ptr);
    }
}

int main() {
    non_const_lvalue_ref();

    const_lvalue_ref();

    universal_ref();

    value();
}
