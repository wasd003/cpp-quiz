#include <atomic>
#include <quiz/base.h>
#include <optional>
#include <thread>

template<typename T, int Cap = 24>
struct bounded_lockless_queue {
private:
    T data[Cap];
    std::atomic<int> head {0}, producer_tail {0}, consumer_tail {0};

public:
    bool push(const T& val) {
        int shadow_producer_tail = producer_tail.load();
        int shadow_consumer_tail;
        do {
            if ((shadow_producer_tail + 1) % Cap == head.load()) return false;
        } while (!producer_tail.compare_exchange_weak(shadow_producer_tail, (shadow_producer_tail + 1) % Cap));
        data[shadow_producer_tail] = val;
        do {
            shadow_consumer_tail = shadow_producer_tail;
        } while (!consumer_tail.compare_exchange_weak(shadow_consumer_tail, (shadow_consumer_tail + 1) % Cap));
        return true;
    }

    std::optional<T> pop() {
        auto shadow_head = head.load();
        T val;
        do {
            if (shadow_head == consumer_tail.load()) return std::nullopt;
            val = data[shadow_head];
        } while (!head.compare_exchange_weak(shadow_head, (shadow_head + 1) % Cap));
        return val;
    }
};

template<typename T>
void print_ans(T&& ans) {
    if (!ans) std::cout << "empty\n";
    else std::cout << *ans << "\n";
}

void single_thread_test() {
    const int n = 10;
    bounded_lockless_queue<int> q;
    for (int i = 0; i < n; i ++ ) {
        q.push(i);
    }
    for (int i = 0; i < n << 1; i ++ ) {
        auto ans = q.pop();
        print_ans(ans);
    }
}

void multi_thread_test() {
    bounded_lockless_queue<int> queue;
    const int n = 5;
    std::vector<std::thread> th;
    for (int i = 0; i < n; i ++ ) {
        th.emplace_back([](int id, bounded_lockless_queue<int>& queue) {
            for (;;) {
                if (id & 1) {
                    queue.push(42);
                } else {
                    auto ans = queue.pop();
                    print_ans(ans);
                }
            }
        }, i, std::ref(queue));
    }
    for (int i = 0; i < n; i ++ ) th[i].join();
}

int main() {
    /* single_thread_test(); */
    multi_thread_test();
}
