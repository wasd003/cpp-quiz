#include <iostream>
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <type_traits>
#include <mutex>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <tuple>
#include <cassert>
#include <type_traits>
#include <thread>
#include <future>
#include <chrono>
#include <queue>
#include <mutex>

struct any {
private:
    struct execution_interface {
        virtual void execute() = 0;

        virtual ~execution_interface() {}
    };

    template<typename Tsk, typename... Args>
    struct execution_engine : public execution_interface {
    private:
        using R = std::invoke_result_t<Tsk, Args...>;
        std::packaged_task<R(Args...)> tsk;
        std::tuple<Args...> args;

    public:
        virtual void execute() override {
            std::apply(tsk, args);
        }

        execution_engine(Tsk _tsk, Args&&... _args) :
            tsk(std::move(_tsk)),
            args(std::forward<Args>(_args)...)
        {}

        virtual ~execution_engine() {}
    };

    std::unique_ptr<execution_interface> pimpl;

public:
    void execute() {
        assert(pimpl);
        pimpl->execute();
    }

    template<typename Tsk, typename... Args>
    any(Tsk tsk, Args&&... args):
        pimpl(std::make_unique<execution_engine<Tsk, Args...>>(
                    std::move(tsk),
                    std::forward<Args>(args)...))
    {}
};

static std::deque<any> vec;
static constinit std::mutex mtx;
static std::condition_variable cv;

template<typename F, typename... Args>
decltype(auto) submit(F f, Args&&... args) {
    using R = std::invoke_result_t<F, Args...>;
    std::packaged_task<R(Args...)> tsk {f};
    auto fut = tsk.get_future();
    {
        std::unique_lock<std::mutex> lk {mtx};
        vec.emplace_back(std::move(tsk), std::forward<Args>(args)...);
    }
    cv.notify_all();
    return fut;
}

void test_case1() {
    auto f = [](int a, int b) {
        std::cout << a << "+" << b << "=" << a + b << std::endl;
        return a + b;
    };

    std::thread worker([]() {
        for (;;) {
            std::unique_lock<std::mutex> lk {mtx};
            cv.wait(lk, []() { return !vec.empty(); });
            while (vec.size()) {
                auto cur = std::move(vec.front());
                vec.pop_front();
                cur.execute();
            }
        }
    });

    auto fut = submit(f, 3, 4);
    auto ret = fut.get();
    std::cout << "real return:" << ret << std::endl;

    worker.join();
}

int main() {
    test_case1();

    return 0;
}
