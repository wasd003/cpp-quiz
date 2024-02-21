#include <quiz/base.h>
#include <thread>

struct spinlock {
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
    void lock() {
        while (flag.test_and_set(std::memory_order_acquire));
    }

    void unlock() {
        flag.clear(std::memory_order_release);
    }
};

int main() {
    const int n = 5;
    int cnt = 0;
    spinlock lk;
    std::vector<std::thread> th;
    for (int i = 0; i < n; i ++ ) {
        th.emplace_back([&cnt, &lk]() {
            lk.lock();
            cnt ++ ;
            lk.unlock();
        });
    }
    for (int i = 0; i < n; i ++ ) th[i].join();
    std::cout << cnt;
    return 0;
}
