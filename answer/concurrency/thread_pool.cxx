#include <cstdlib>
#include <functional>
#include <iostream>
#include <atomic>
#include <array>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <memory>
#include <cassert>
#include <new>
#include <pthread.h>
#include <thread>
#include <future>
#include <utility>
#include <vector>
#include <type_traits>
#include <string_view>
#include <cstring>
#include <unordered_map>
#include <optional>
#include <shared_mutex>

using namespace std;

constexpr static int N = 12;
constexpr static int M = 1024;

std::vector<std::packaged_task<int(void)>> wqs[N];
std::mutex mtxs[N];
std::condition_variable cvs[N];
bool stop_flag[N];

int select_worker() {
    static int cur = 0;
    auto ans = cur;
    cur = (cur + 1) % N;
    return ans;
}

template<typename F, typename... Args>
std::future<int> submit(F&& func, Args&&... args) {
    std::packaged_task<int(void)> tsk([_func = std::forward<F>(func), ..._args = std::forward<Args>(args)]() {
        return _func(_args...);
    });
    const auto worker_id = select_worker();
    auto& cv = cvs[worker_id];
    auto& mtx = mtxs[worker_id];
    auto& wq = wqs[worker_id];
    std::unique_lock<std::mutex> lk(mtx);
    wq.emplace_back(std::move(tsk));
    auto ans = wq.back().get_future();
    cv.notify_all();
    return ans;
}

void worker_fn(int worker_id) {
    auto& cv = cvs[worker_id];
    auto& mtx = mtxs[worker_id];
    auto& wq = wqs[worker_id];
    auto& stop = stop_flag[worker_id];
    for (;;) {
        std::unique_lock<std::mutex> lk(mtx);
        cv.wait(lk, [&wq, &stop]() { return wq.size() || stop; });
        while (wq.size()) {
            auto& cur_tsk = wq.back();
            cur_tsk();
            wq.pop_back();
        }
        if (stop)
            break;
    }
}

int main() {
    std::vector<std::thread> worker_list;
    for (int i = 0; i < N; i ++ ) {
        worker_list.emplace_back(worker_fn, i);
    }

    std::vector<std::future<int>> results;
    for (int i = 0; i < M; i ++ ) {
        auto fut = submit([](int a, int b) { return a + b; }, i, i);
        results.emplace_back(std::move(fut));
    }
    for (int i = 0; i < M; i ++ ) {
        auto ans = results[i].get();
        std::cout << "i:" << i << " ans:" << ans << std::endl;
    }

    for (int i = 0; i < N; i ++ ) {
        stop_flag[i] = true;
        cvs[i].notify_all();
        worker_list[i].join();
    }
    return 0;
}

