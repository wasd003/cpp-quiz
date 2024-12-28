#include <iterator>
#include <quiz/base.h>
#include <quiz/log.h>
#include <type_traits>

template<typename T>
std::enable_if_t<std::is_trivial_v<T>> t1_func(const T&) {
    debug("trivial version");
};

template<typename T>
std::enable_if_t<!std::is_trivial_v<T>> t1_func(const T&) {
    debug("non trivial version");
};

template<typename T>
concept trivial_cpt = std::is_trivial_v<T>;

template<typename T>
void t1_func_cpt(const T&) {
    debug("non trivial version");
}

template<typename T>
requires trivial_cpt<T>
void t1_func_cpt(const T&) {
    debug("trivial version");
}

void T1() {
    std::vector v {1, 2, 3};
    t1_func(v);
    t1_func(42);

    t1_func_cpt(v);
    t1_func_cpt(42);
}

void t2_func_aux(std::random_access_iterator_tag) {
    debug("random access");
}

void t2_func_aux(std::bidirectional_iterator_tag) {
    debug("bidirectional");
}

template<typename Iter>
void t2_func(Iter iter) {
    t2_func_aux(typename std::iterator_traits<Iter>::iterator_category {});
}

template<typename Iter>
requires requires {
    requires std::is_same_v<std::random_access_iterator_tag,
             typename std::iterator_traits<Iter>::iterator_category>;
}
void t2_func_cpt(Iter iter) {
    debug("random access");
}

template<typename Iter>
requires requires {
    requires std::is_same_v<std::bidirectional_iterator_tag,
             typename std::iterator_traits<Iter>::iterator_category>;
}
void t2_func_cpt(Iter iter) {
    debug("bidirectional");
}

void T2() {
    std::vector v {1, 2, 3};
    std::list l {1, 2, 3};
    auto random_access_iter = std::begin(v);
    auto bidirectional_iter = std::begin(l);
    t2_func(random_access_iter);
    t2_func(bidirectional_iter);
    t2_func_cpt(random_access_iter);
    t2_func_cpt(bidirectional_iter);
}

template<typename T>
void t3_func_aux(const T&, long) {
    debug("no ok");
}

template<typename T, typename = decltype(std::declval<T>().ok(int {}, int {}))>
void t3_func_aux(const T&, int) {
    debug("has ok");
}

template<typename T>
void t3_func(const T& args) {
    t3_func_aux(args, 1);
}

template<typename T>
concept has_ok_cpt = requires(T t) {
    {t.ok(int{}, int{})} -> std::same_as<void>;
};

template<typename T>
void t3_func_cpt(const T&) {
    debug("no ok");
}

template<typename T>
requires has_ok_cpt<T>
void t3_func_cpt(const T&) {
    debug("has ok");
}

template<typename T>
void t3_func_constexpr(const T& args) {
    if constexpr (requires(T t) {
        {t.ok(int{}, int{})} -> std::same_as<void>;
    }) {
        debug("has ok");
    } else {
        debug("no ok");
    }
}

void T3() {
    struct has_ok_type { void ok(int, int) {}};
    struct no_ok_type {};
    t3_func(has_ok_type {});
    t3_func(no_ok_type {});
    t3_func_cpt(has_ok_type {});
    t3_func_cpt(no_ok_type {});
    t3_func_constexpr(has_ok_type {});
    t3_func_constexpr(no_ok_type {});
}

int main() {
    T1();
    T2();
    T3();
}
