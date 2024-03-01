#include <mutex>
#include <quiz/base.h>
#include <thread>
#include <condition_variable>

constinit std::mutex mtx;
int hcnt = 0, ocnt = 0;
std::condition_variable hcond, ocond;

inline bool check_final_round() {
    return hcnt == 2 && ocnt == 1;
}

inline void reset() {
    hcnt = ocnt = 0;
}

int main() {
    std::thread hthread([]() {
        for (;;) {
            std::unique_lock<std::mutex> lk(mtx);
            hcond.wait(lk, []() { return hcnt < 2; });
            printf("H");
            hcnt ++ ;
            if (check_final_round()) reset();
            ocond.notify_all();
        }
    });
    std::thread othread([]() {
        for (;;) {
            std::unique_lock<std::mutex> lk(mtx);
            ocond.wait(lk, []() { return ocnt < 1; });
            printf("O");
            ocnt ++ ;
            if (check_final_round()) reset();
            hcond.notify_all();
        }
    });
    hthread.join();
    othread.join();
    return 0;
}
