#include <atomic>
#include <quiz/base.h>
#include <optional>
#include <thread>
#include <sstream>
#include <syncstream>
#include <sys/syscall.h>
#include <unistd.h>
#include <unordered_map>
#include <array>

struct producer_log_entry {
    int shadow_producer_tail;
    int local_head;

    producer_log_entry(int shadow_producer_tail, int local_head)
        : shadow_producer_tail(shadow_producer_tail), local_head(local_head) {}
};

struct consumer_log_entry {
    int shadow_head;
    int local_consumer_tail;

    consumer_log_entry(int shadow_head, int local_consumer_tail)
        : shadow_head(shadow_head), local_consumer_tail(local_consumer_tail) {}
};

std::array<std::vector<producer_log_entry>, 10> producer_logs;
std::array<std::vector<consumer_log_entry>, 10> consumer_logs;

template<typename T, int Cap = 24>
struct bounded_lockless_queue {
private:
    T data[Cap];
    std::atomic<int> head {0}, producer_tail {0}, consumer_tail {0};

public:
    bool push(const T& val, pid_t pid) {
        int shadow_producer_tail = producer_tail.load();
        int shadow_consumer_tail;
        int local_head;
        do {
            local_head = head.load();
            auto current_size = shadow_producer_tail - local_head;
            assert(current_size <= Cap);
            if (current_size == Cap) return false;
        } while (
            (producer_tail.load(std::memory_order_relaxed) != shadow_producer_tail)
            ||
            (!producer_tail.compare_exchange_weak(shadow_producer_tail, shadow_producer_tail + 1)));
        producer_logs[pid].emplace_back(shadow_producer_tail, local_head);
        data[shadow_producer_tail % Cap] = val;
        do {
            shadow_consumer_tail = shadow_producer_tail;
        } while (!consumer_tail.compare_exchange_weak(shadow_consumer_tail, shadow_consumer_tail + 1));
        return true;
    }

    std::optional<T> pop(const pid_t pid) {
        int shadow_head = head.load();
        T val;
        int local_consumer_tail;
        do {
            local_consumer_tail = consumer_tail.load();
            if (shadow_head == local_consumer_tail) return std::nullopt;
            assert(shadow_head < local_consumer_tail);
            consumer_logs[pid].emplace_back(shadow_head, local_consumer_tail);
            val = data[shadow_head % Cap];
        } while (!head.compare_exchange_weak(shadow_head, shadow_head + 1));
        return val;
    }
};

template<typename T>
void print_ans(T&& ans) {
    if (!ans) std::cout << "empty\n";
    else std::cout << *ans << "\n";
}

// void single_thread_test() {
//     const int n = 10;
//     bounded_lockless_queue<int> q;
//     for (int i = 0; i < n; i ++ ) {
//         q.push(i);
//     }
//     for (int i = 0; i < n << 1; i ++ ) {
//         auto ans = q.pop();
//         print_ans(ans);
//     }
// }

void multi_thread_test() {
    bounded_lockless_queue<int> queue;
    const int n = 5;
    std::vector<std::thread> th;
    for (int i = 0; i < n; i ++ ) {
        th.emplace_back([](int id, bounded_lockless_queue<int>& queue) {
            const pid_t tid = syscall(SYS_gettid);
            std::cout << "Thread ID: " << tid << " Custom ID:" << id << std::endl;
            for (;;) {
                if (id & 1) {
                    queue.push(42, id);
                } else {
                    auto ans = queue.pop(id);
                    UNUSED(ans);
                    // print_ans(ans);
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
