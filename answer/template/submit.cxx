#include <iostream>
#include <atomic>
#include <array>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <memory>
#include <cassert>
#include <pthread.h>
#include <thread>
#include <future>
#include <vector>
#include <type_traits>


int num_add(int a, int b) {
    return a + b;
}

std::string str_add(const std::string& a, const std::string& b) {
    return a + b;
}

std::vector<std::packaged_task<int(void)>> basic_wq;
std::vector<std::function<void(void)>> advance_wq;

template<typename F, typename... Args>
decltype(auto) basic_submit(F f, Args&&... args) {
    std::packaged_task<int(void)> tsk {
        [_f = std::move(f), ..._args = std::forward<Args>(args)]() {
            return _f(_args...);
        }
    };
    basic_wq.push_back(std::move(tsk));
    return basic_wq.back().get_future();
}

template<typename F, typename... Args>
decltype(auto) advance_submit(F f, Args&&... args) {
    using result_type = std::invoke_result_t<F, Args...>;

    auto tsk = std::make_shared<std::packaged_task<result_type(void)>> (
        [_f = std::move(f), ..._args = std::forward<Args>(args)]() {
            return _f(_args...);
        }
    );

    std::function<void(void)> func {
        [_tsk = tsk]() {
            (*_tsk)();
        }
    };

    advance_wq.push_back(std::move(func));
    return tsk->get_future();
}

int main() {
    auto fut = basic_submit(num_add, 1, 2);
    basic_wq[0]();
    auto ans = fut.get();
    std::cout << ans << std::endl;

    auto fut2 = advance_submit(str_add, "abc", "def");
    advance_wq[0]();
    auto ans2 = fut2.get();
    std::cout << ans2 << std::endl;
    return 0;
}

