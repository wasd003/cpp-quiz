#include <x86intrin.h>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <atomic>
#include <array>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <memory>
#include <cassert>
#include <ostream>
#include <thread>
#include <random>
#include <ctime>
#include <chrono>
#include <utility>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fstream>
#include <cstring>
#include <vector>

using namespace std;

struct HashAlgo1 {
    static size_t hash(size_t val, const char* data, size_t len) {
        for (size_t i = 0; i < len; i ++ ) {
            val ^= data[i];
        }
        return val;
    }
};

struct HashAlgo2 {
    static size_t hash(size_t val, const char* data, size_t len) {
        for (size_t i = 0; i < len; i ++ ) {
            val *= data[i];
        }
        return val;
    }
};

template<typename T>
struct is_std_vector { constexpr static bool value = false; };

template<typename Elem, typename Alloc>
struct is_std_vector<std::vector<Elem, Alloc>> { constexpr static bool value = true; };

template<typename T>
consteval bool direct_hash() {
    using type = std::remove_cvref_t<T>;
    if constexpr(std::is_fundamental_v<type> || std::is_pointer_v<type> || std::is_enum_v<type>)
        return true;
    if constexpr (std::is_same_v<std::string, T>)
        return true;
    if constexpr (is_std_vector<type>::value)
        return true;
    return false;
}

template<typename HashAlgo>
class Hasher {
private:
    size_t val {345235346745134}; /// default magic number

public:
    Hasher(size_t _val) : val(_val) {}

    Hasher() = default;

    template<typename First, typename... Rest>
    void operator()(const First& first, const Rest&... rest) {
        hash_one(first);
        (*this)(rest...);
    }

    void operator()() {}

    template<typename T>
    void hash_one(const T& args) {
        std::cout << "hash_one:" << args << std::endl;
        if constexpr (direct_hash<T>()) {
            val = HashAlgo::hash(val, reinterpret_cast<const char*>(&args), sizeof(T));
        } else {
            static_assert(requires { T::enumerate_members(args, *this); }, "unsupported type");
            T::enumerate_members(args, *this);
        }
    }

    operator size_t() {
        return val;
    }
};

struct B {
    int x;
    double y;

    static void enumerate_members(auto&& self, auto&& hasher) {
        hasher(self.x, self.y);
    }

    friend std::ostream& operator<<(std::ostream& os, const B& b) {
        os << "B:[" << b.x << ' ' << b.y << ']';
        return os;
    }
};

struct A {
    std::string s;
    B b;

    static void enumerate_members(auto&& self, auto&& hasher) {
        hasher(self.s, self.b);
    }

    friend std::ostream& operator<<(std::ostream& os, const A& a) {
        os << "A:[";
        os << a.s << ' ' << a.b << ' ';
        os << ']';
        return os;
    }
};

int main() {
    Hasher<HashAlgo1> hasher;
    A a {"hello", {1, 2.0}};
    hasher(a);
}