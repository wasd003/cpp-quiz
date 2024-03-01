#include <thread>
#include <optional>
#include <quiz/base.h>
#include <quiz/log.h>

template<typename T>
struct node {
    T data;
    node<T> *next;

    node(const T& data) : data(data) {}
};

template<typename T>
struct lockless_stack {
    std::atomic<node<T>*> head {nullptr};

    void push(const T& val) {
        auto new_node = new node<T>(val);
        auto shadow_head = head.load();
        do {
            new_node->next = shadow_head;
        } while (!head.compare_exchange_strong(shadow_head, new_node));
    }

    std::optional<T> pop() {
        auto shadow_head = std::atomic_load(&head);
        do {
            if (!shadow_head) return std::nullopt;
        } while (!head.compare_exchange_strong(shadow_head, shadow_head->next));
        auto val = std::move(shadow_head->data);
        delete shadow_head;
        return val;
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
    /* single_thread_test(); */

    multi_thread_test();
}
