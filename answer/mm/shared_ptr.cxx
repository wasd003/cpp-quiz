#include <cassert>
#include <memory>
#include <quiz/base.h>
#include <atomic>

template<typename Del>
struct shared_ptr_control_block {
    std::atomic<int> count;
    Del del;

    shared_ptr_control_block(int cnt, Del del) : count(cnt), del(del) {}
};

template<typename T>
struct default_del_op {
    void operator()(T* ptr) {
        std::cout << "del";
        delete ptr;
    }
};

template<typename T>
struct test_del_op {
    void operator()(T* ptr) {
        std::cout << "test del";
        delete ptr;
    }
};

template<typename T, typename Del = default_del_op<T>>
struct shared_ptr {
    T *ptr;
    shared_ptr_control_block<Del> *cb;

    shared_ptr(T* ptr, const Del& del = Del {}) : ptr(ptr), cb(new shared_ptr_control_block {1, del}) {}

    shared_ptr(const shared_ptr& rhs) : ptr(rhs.ptr), cb(rhs.cb) {
        assert(cb);
        cb->count ++ ;
    }

    shared_ptr& operator=(const shared_ptr& rhs) {
        auto tmp = rhs;
        swap(tmp);
        return *this;
    }

    shared_ptr(shared_ptr&& rhs) : ptr(rhs.ptr), cb(rhs.cb) {
        assert(cb);
        rhs.ptr = nullptr;
        rhs.cb = nullptr;
    }

    shared_ptr& operator=(shared_ptr&& rhs) {
        auto tmp = std::move(rhs);
        swap(tmp);
        return *this;
    }

    void swap(shared_ptr& rhs) noexcept {
        swap(ptr, rhs.ptr);
        swap(cb, rhs.cb);
    }

    ~shared_ptr() {
        if ( -- cb->count == 0) {
            cb->del(ptr);
        }
    }
};

struct test {
    int a, b, c;
};

int main() {
    auto ptr = new test {1, 2, 3};
    auto sp = shared_ptr<test, test_del_op<test>>(ptr);
    return 0;
}
