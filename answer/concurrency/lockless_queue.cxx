#include <optional>
#include <atomic>
#include <thread>
#include <quiz/log.h>
#include <quiz/base.h>

template<typename T>
struct node {
    node *next;
    T data;

    node(const T& data) : next(nullptr), data(data) {}

    node() {}
};

template<typename T>
struct lockless_queue {
    using PTR = node<T>*;
    std::atomic<PTR> head;
    std::atomic<PTR> tail;

    lockless_queue() : head(new node<T>{}), tail(head.load()) {}

    void push(const T& data) {
        auto new_node = new node<T>(data);
        auto shadow_tail = tail.load();
        while (!tail.compare_exchange_weak(shadow_tail, new_node));
        shadow_tail->next = new_node;
    }

    // FIXME: memory leak
    std::optional<T> pop() {
        auto shadow_head = head.load();
        bool empty;
        while (!(empty = (shadow_head == tail.load())) && !head.compare_exchange_weak(shadow_head, shadow_head->next));
        if (empty) return std::nullopt;
        else {
            auto ans = shadow_head->next->data;
            return ans;
        }
    }
};

template<typename T>
void print_ans(T&& ans) {
    if (!ans) std::cout << "empty\n";
    else std::cout << *ans << "\n";
}

void single_thread_test() {
    const int n = 10;
    lockless_queue<int> q;
    for (int i = 0; i < n; i ++ ) {
        q.push(i);
    }
    for (int i = 0; i < n << 1; i ++ ) {
        auto ans = q.pop();
        print_ans(ans);
    }
}

void multi_thread_test() {
    lockless_queue<int> stk;
    const int n = 5;
    std::vector<std::thread> th;
    for (int i = 0; i < n; i ++ ) {
        th.emplace_back([](int id, lockless_queue<int>& stk) {
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

int main() {
    multi_thread_test();
}
