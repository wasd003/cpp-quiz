#include <cassert>
#include <quiz/base.h>
#include <quiz/log.h>
#include <atomic>

struct control_block {
    std::atomic<int> use_count;
    std::atomic<int> weak_count;

    control_block(int uc, int wc) : use_count(uc), weak_count(wc) {}
};

template<typename T>
struct shared_ptr {
    T* ptr;
    control_block *cb;

    // this demo focus on weak_ptr, simplify shared_ptr
    shared_ptr(const shared_ptr&) = delete;
    shared_ptr& operator=(const shared_ptr&) = delete;
    shared_ptr(shared_ptr&&) = delete;
    shared_ptr& operator=(shared_ptr&&) = delete;

    T* operator->() {
        return ptr;
    }

    shared_ptr() : ptr(nullptr), cb(nullptr) {}

    shared_ptr(T* ptr) : ptr(ptr), cb(new control_block {1, 1}) {}

    shared_ptr(T* ptr, control_block* cb) : ptr(ptr), cb(cb) {}

    void reset() {
        if (!cb) {
            assert(!ptr);
            return;
        }

        if ( -- cb->use_count == 0) {
            if ( -- cb->weak_count == 0) {
                debug("delete cb");
                delete cb;
                cb = nullptr;
            }
            debug("delete ptr");
            delete ptr;
            ptr = nullptr;
        }
    }

    ~shared_ptr() {
        reset();
    }
};

template<typename T>
struct weak_ptr {
    T *ptr;
    control_block *cb;
    weak_ptr(const shared_ptr<T>& sp) : ptr(sp.ptr), cb(sp.cb) {
        cb->weak_count ++ ;
    }

    bool expired() {
        return cb->use_count == 0;
    }

    shared_ptr<T> lock() {
        auto uc = cb->use_count.load();
        while (uc && !cb->use_count.compare_exchange_weak(uc, uc + 1));
        if (uc) {
            return shared_ptr<T> {ptr, cb};
        } else {
            return shared_ptr<T> {};
        }
    }

    ~weak_ptr() {
        if ( -- cb->weak_count == 0) {
            debug("delete cb");
            delete cb;
            cb = nullptr;
        }
    }

    weak_ptr(const weak_ptr& rhs) : ptr(rhs.ptr), cb(rhs.cb) {
        cb->weak_count ++ ;
    }
    weak_ptr(weak_ptr&& rhs) : ptr(rhs.ptr), cb(rhs.cb) {
        rhs.ptr = nullptr;
        rhs.cb = nullptr;
    }

    weak_ptr& operator=(const weak_ptr&) = delete;
    weak_ptr& operator=(weak_ptr&&) = delete;
};

struct test {
    int a, b, c;
};

void test_case1() {
    auto ptr = new test {1, 2, 3};
    auto sp =  shared_ptr<test>(ptr);
    auto wp = weak_ptr<test>(sp);
    {
        auto wp2 = wp;
    }
}

void test_case2() {
    auto ptr = new test {1, 2, 3};
    auto sp =  shared_ptr<test>(ptr);
    auto wp = weak_ptr<test>(sp);
    sp.reset();
    assert(wp.expired());
}

int main() {
    test_case1();
    test_case2();
    return 0;
}
