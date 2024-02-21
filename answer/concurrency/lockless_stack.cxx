#include <thread>
#include <optional>
#include <quiz/base.h>
#include <quiz/log.h>

template<typename T>
struct node {
    node *prev;
    T data;

    node(const T& data) : prev(nullptr), data(data) {}

    node() {}
};

template<typename T>
struct lockless_stack {
    using PTR = node<T>*;
    std::atomic<PTR> head;

    lockless_stack() : head(new node<T> {}) {}

    void push(const T& data) {
        auto shadow_head = head.load();
        auto new_head = new node(data);
        new_head->prev = shadow_head;
        while (!head.compare_exchange_weak(shadow_head, new_head)) {
            new_head->prev = shadow_head;
        }
    }

    std::optional<T> pop() {
        auto shadow_head = head.load();
        bool ans_flag;
        while ((ans_flag = (shadow_head->prev != nullptr)) && !head.compare_exchange_weak(shadow_head, shadow_head->prev));
        if (ans_flag) {
            return std::move(shadow_head->data);
        } else {
            return std::nullopt;
        }
    }
};

template<typename T>
requires requires(T t) {
    assert(t);
    *t;
}
void print_ans(T&& ans) {
    if (!ans) std::cout << "empty\n";
    else std::cout << *ans << "\n";
}

void multi_thread_test() {
    lockless_stack<int> stk;
    const int n = 5;
    std::vector<std::thread> th;
    for (int i = 0; i < n; i ++ ) {
        th.emplace_back([](int id, lockless_stack<int>& stk) {
            for (;;) {
                if (id & 1) {
                    stk.push(42);
                } else {
                    auto ans = stk.pop();
                    print_ans(ans);
                }
            }
        }, i, std::ref(stk));
    }
    for (int i = 0; i < n; i ++ ) th[i].join();
}

void single_thread_test() {
    const int n = 10;
    lockless_stack<int> stk;
    for (int i = 0; i < n; i ++ ) {
        stk.push(i);
    }
    for (int i = 0; i < n << 1; i ++ ) {
        auto ans = stk.pop();
        print_ans(ans);
    }
}

int main() {
    single_thread_test();

    multi_thread_test();
}
