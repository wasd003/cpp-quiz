#include <quiz/base.h>
#include <atomic>
#include <thread>

struct fail_spinlock {
    std::atomic<int> seq;
    std::atomic<int> flag;

    constexpr fail_spinlock() : seq(0), flag{0} {}

    void lock() {
        int myseq = seq.fetch_add(1);
        while (flag.load() != myseq);
    }

    void unlock() {
        flag ++ ;
    }
};

constinit fail_spinlock lk;

int count = 0;

int main() {
    std::vector<std::thread> thread_list;
    for (int i = 0; i < 12; i ++ ) {
        thread_list.emplace_back([]() {
            for (int j = 0; j < 100000; j ++ ) {
                lk.lock();
                count ++ ;
                lk.unlock();
            }
        });
    }

    for (auto& th : thread_list) th.join();
    std::cout << count << std::endl;
    return 0;
}
