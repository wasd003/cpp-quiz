#include <atomic>
#include <mutex>
#include <quiz/base.h>
#include <thread>
#include <condition_variable>

void condition_variable_ver() {
    constinit static std::mutex mtx;
    int hcnt = 0, ocnt = 0;
    std::condition_variable hcond, ocond;

    auto check_final_round = [&hcnt, &ocnt]() {
        return hcnt == 2 && ocnt == 1;
    };

    auto reset = [&hcnt, &ocnt]() {
        hcnt = ocnt = 0;
    };

    // FIXME: Bad Practice: don't use default capture !!
    std::thread hthread([&]() {
        for (;;) {
            std::unique_lock<std::mutex> lk(mtx);
            hcond.wait(lk, [&hcnt]() { return hcnt < 2; });
            printf("H");
            hcnt ++ ;
            if (check_final_round()) reset();
            ocond.notify_all();
        }
    });
    // FIXME: Bad Practice: don't use default capture !!
    std::thread othread([&]() {
        for (;;) {
            std::unique_lock<std::mutex> lk(mtx);
            ocond.wait(lk, [&ocnt]() { return ocnt < 1; });
            printf("O");
            ocnt ++ ;
            if (check_final_round()) reset();
            hcond.notify_all();
        }
    });
    hthread.join();
    othread.join();
}

void atomic_variable_ver() {
    std::atomic<uint8_t> x {0}; // ocnt(1-bit) | hcnt(2-bit)
    constinit static std::mutex mtx;
    setbuf(stdout, NULL);
    std::thread hthread([&x]() {
        for (;;) {
            auto shadowx = x.load();
            bool flag;
            uint8_t newx;
            do {
                int hcnt = shadowx & 3;
                if (hcnt == 2) {
                    flag = false;
                    break;
                } else {
                    assert(hcnt < 2);
                    newx = shadowx + 1;
                    flag = true;
                }
            } while (!x.compare_exchange_weak(shadowx, newx));
            std::unique_lock<std::mutex> lk(mtx);
            if (flag) printf("H");
        }
    });

    std::thread othread([&x]() {
        for (;;) {
            auto shadowx = x.load();
            bool flag;
            uint8_t newx;
            do {
                int hcnt = shadowx & 3, ocnt = shadowx >> 2;
                if (hcnt == 2 && ocnt == 1) {
                    flag = false;
                    newx = 0;
                } else if (ocnt == 1) {
                    flag = false;
                    break;
                } else {
                    assert(ocnt == 0);
                    newx = shadowx | (1 << 2);
                    flag = true;
                }
            } while (!x.compare_exchange_weak(shadowx, newx));
            std::unique_lock<std::mutex> lk(mtx);
            if (flag) printf("O");
        }
    });
    hthread.join();
    othread.join();
}

int main() {
    condition_variable_ver();
    atomic_variable_ver();
    return 0;
}
