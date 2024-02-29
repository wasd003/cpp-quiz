#include <cassert>
#include <mutex>
#include <quiz/base.h>
#include <atomic>
#include <thread>

struct group_lock {
    std::atomic<uint32_t> lk {0};

    enum class lock_state {
        UNLOCK,
        GROUP1_LOCK,
        GROUP2_LOCK,
    };

    lock_state get_lock_state(uint32_t lk_val) {
        int state_bit = lk_val >> 30;
        switch (state_bit) {
        case 0:
            return lock_state::UNLOCK;
        case 1:
            return lock_state::GROUP1_LOCK;
        case 2:
            return lock_state::GROUP2_LOCK;
        default:
            assert(0);
        }
    }

    uint32_t set_new_lock(uint32_t old_lk_val, lock_state new_state) {
        auto state = static_cast<std::underlying_type_t<lock_state>>(new_state);
        assert(state >= 0 && state < 3);
        return old_lk_val | (state << 30);
    }

    uint32_t get_count(uint32_t lk_val) {
        return lk_val & ((1UL << 30) - 1);
    }

    bool lock(lock_state group) {
        auto shadow_lk = lk.load();
        uint32_t new_lk_val;
        do {
            auto state = get_lock_state(shadow_lk);
            if (state == lock_state::UNLOCK) {
                new_lk_val = set_new_lock(shadow_lk, group) + 1;
            } else if (state != group) return false;
            else {
                assert(state == group);
                new_lk_val = shadow_lk + 1;
            }
        } while (!lk.compare_exchange_weak(shadow_lk, new_lk_val));
        return true;
    }

    void unlock(lock_state group) {
        auto shadow_lk = lk.load();
        uint32_t new_lk_val;
        do {
            auto state = get_lock_state(shadow_lk);
            assert(state == group);
            int new_cnt = get_count(shadow_lk - 1);
            if (new_cnt == 0) {
                new_lk_val = 0;
            } else {
                new_lk_val = shadow_lk - 1;
            }
            // printf("new_cnt:%d state:%d -> %d\n", new_cnt, 
            //         static_cast<int>(state),
            //         static_cast<int>(get_lock_state(new_lk_val)));
        } while (!lk.compare_exchange_weak(shadow_lk, new_lk_val));
    }
};

int main() {
    group_lock lk;
    int cnt = 0;
    std::thread th1([&lk, &cnt]() {
        for (int i = 0; i < 10000; i ++ ) {
            while (!lk.lock(group_lock::lock_state::GROUP1_LOCK));
            cnt ++ ;
            lk.unlock(group_lock::lock_state::GROUP1_LOCK);
        }
    });
    std::thread th2([&lk, &cnt]() {
        for (int i = 0; i < 10000; i ++ ) {
            while (!lk.lock(group_lock::lock_state::GROUP2_LOCK));
            cnt ++ ;
            lk.unlock(group_lock::lock_state::GROUP2_LOCK);
        }
    });
    th1.join();
    th2.join();
    printf("cnt:%d\n", cnt);
    return 0;
}
