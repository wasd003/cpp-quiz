#include <quiz/base.h>
#include <atomic>
#include <array>
#include <vector>
#include <thread>
#include <iostream>

/// refer: https://www.cnblogs.com/LoyenWang/p/12681494.html

template<typename T>
struct RCU {
private:
    constexpr static int MAX_GENERATIONS = 1024;

private:
    std::atomic<int> last_generation {0};
    int last_gc_genration {0};
    std::array<std::atomic<int>, MAX_GENERATIONS> refcnt;
    std::array<T*, MAX_GENERATIONS> data;

public:
    struct rcu_handle {
    private:
        int generation;
    public:
        int get() const { return generation; }
        rcu_handle(int generation) : generation(generation) {}
    };

    /// API for Reader
    rcu_handle rcu_read_lock() {
        int current_generation = last_generation.load() - 1;
        refcnt[current_generation] ++ ;
        while(current_generation != last_generation.load() - 1) { // unlikely
            refcnt[current_generation] -- ;
            current_generation = last_generation.load() - 1;
            refcnt[current_generation] ++ ;
        }
        return rcu_handle {current_generation};
    }

    void rcu_read_unlock(const rcu_handle& handle) {
        refcnt[handle.get()] -- ;
    }

    T *rcu_dereference(const rcu_handle& handle) {
        return data[handle.get()];
    }

    /// API for Writer
    void synchronize_rcu() {
        while (last_gc_genration < last_generation.load() - 1) {
            while (refcnt[last_gc_genration] != 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            std::cout << "GC: " << last_gc_genration << std::endl;
            delete data[last_gc_genration];
            last_gc_genration ++;
        }
    }

    void rcu_assign_pointer(T *new_data) {
        data[last_generation] = new_data;
        last_generation ++;
    }
};

int main() {
    RCU<int> rcu;
    std::vector<std::thread> readers;
    rcu.rcu_assign_pointer(new int {-1});

    for (int i = 0; i < 10; i++) {
        readers.emplace_back([&rcu](){
            for (;;) {
                auto handle = rcu.rcu_read_lock();
                auto data_ptr = rcu.rcu_dereference(handle);
                assert(data_ptr);
                auto data = *data_ptr;
                UNUSED(data);
                // std::cout << *data << std::endl;
                rcu.rcu_read_unlock(handle);
            }
        });
    }

    std::thread writer(
        [&rcu](){
            for (int i = 0; i < 10; i++) {
                rcu.rcu_assign_pointer(new int(i));
                rcu.synchronize_rcu();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            std::cout << "Writer Done" << std::endl;
        }
    );
    for (size_t i = 0; i < readers.size(); i++) {
        readers[i].join();
    }
    writer.join();
    return 0;
}