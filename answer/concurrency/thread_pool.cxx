#include <condition_variable>
#include <mutex>
#include <quiz/base.h>
#include <functional>
#include <type_traits>

constexpr static int worker_nr = 12;
std::queue<std::function<int(int)>> wq[worker_nr];
std::queue<int> cq;

std::mutex wq_mtx[worker_nr];
std::condition_variable wq_cond[worker_nr];

std::mutex cq_mtx;
std::condition_variable cq_cond;

int select_worker() {
    static int cur = 0;
    auto ans = cur;
    cur = (cur + 1) % worker_nr;
    return ans;
}

bool can_run() {
    return true;
}

int main() {
    std::vector<std::thread> worker_list;
    const int max_seq_submit_cnt = 5;
    std::thread scheduler([]() {
        for (;;) {
            for (int round = 0; round < max_seq_submit_cnt; round ++ ) {
                auto worker_idx = select_worker();
                std::unique_lock<std::mutex> lk(wq_mtx[worker_idx]);
                wq[worker_idx].emplace([](int i) {
                        return i * 17;
                });
                printf("[sched] wakeup worker-%d\n", worker_idx);
                wq_cond[worker_idx].notify_all();
            }
            std::unique_lock<std::mutex> lk(cq_mtx);
            cq_cond.wait(lk, []() {return cq.size() >= max_seq_submit_cnt;});
            while (cq.size()) {
                auto result = cq.front(); cq.pop();
                printf("[sched] result:%d\n", result);
            }
        }
    });

    for (int i = 0; i < worker_nr; i ++ ) {
        worker_list.emplace_back([](int worker_id) {
            auto& my_wq = wq[worker_id];
            std::unique_lock<std::mutex> lk(wq_mtx[worker_id]);
            for (;;) {
                wq_cond[worker_id].wait(lk, [&my_wq]() { return my_wq.size(); });
                while (my_wq.size()) {
                    auto job = my_wq.front(); my_wq.pop();
                    auto result = job(10);
                    printf("[worker-%d] result:%d\n", worker_id, result);
                    std::unique_lock<std::mutex> cq_lk(cq_mtx);
                    cq.emplace(result);
                }
                cq_cond.notify_all();
            }
        }, i);
    }
    scheduler.join();
    for (auto& i : worker_list) i.join();
}
