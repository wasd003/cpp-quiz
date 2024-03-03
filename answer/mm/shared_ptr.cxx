#include <cassert>
#include <memory>
#include <quiz/base.h>
#include <atomic>
#include <functional>

template<typename T>
struct default_deleter {
    void operator()(T *ptr) {
        std::cout << "default_deleter";
        delete ptr;
    }
};

template<typename T>
struct control_block {
    std::atomic<int> refcnt;
    std::function<void(T*)> deleter;

    control_block(int refcnt, std::function<void(T*)> deleter):
        refcnt(refcnt), deleter(deleter) {}
};

template<typename T>
struct shared_ptr {
    T *data;
    control_block<T> *cb;

    shared_ptr(T *data, std::function<void(T*)> deleter = default_deleter<T> {}):
        data(data), cb(new control_block<T> {1, deleter}) {}

    shared_ptr() : data(nullptr), cb(nullptr) {}

    T *get() {
        return data;
    }

    T *operator->() {
        return data;
    }

    void swap(shared_ptr &rhs) {
        std::swap(data, rhs.data);
        std::swap(cb, rhs.cb);
    }

    shared_ptr(const shared_ptr& rhs) : data(rhs.data), cb(rhs.cb) {
        cb->refcnt ++ ;
    }

    shared_ptr& operator=(const shared_ptr& rhs) {
        auto tmp = rhs;
        swap(tmp);
        return *this;
    }

    shared_ptr(shared_ptr&& rhs) : data(rhs.data), cb(rhs.cb) {
        rhs.data = nullptr;
        rhs.cb = nullptr;
    }

    shared_ptr& operator=(shared_ptr&& rhs) {
        auto tmp = std::move(rhs);
        swap(tmp);
        return *this;
    }

    ~shared_ptr() {
        auto oldcnt = cb->refcnt.fetch_sub(1);
        if (oldcnt == 1) {
            cb->deleter(data);
            data = nullptr;
            delete cb;
            cb = nullptr;
        }
    }

    operator bool() {
        return data != nullptr;
    }
};

struct test {
    int a, b, c;
};

template<typename T>
struct test_del_op {
    void operator()(T* ptr) {
        std::cout << "test_del_op";
        delete ptr;
    }
};


template<typename T, typename... Args>
shared_ptr<T> make_shared(Args&&... args) {
    auto ptr = new uint8_t [sizeof (T) + sizeof (control_block<T>)];
    auto sp = shared_ptr<T> {};
    sp.data = reinterpret_cast<T*>(ptr);
    sp.cb = reinterpret_cast<control_block<T>*>(ptr + sizeof (T));
    new(ptr) T(std::forward<Args>(args)...);
    return sp;
}

int main() {
    auto ptr = new test {1, 2, 3};
    auto sp = shared_ptr<test>(ptr, test_del_op<test>{});

    using PII = std::pair<int, int>;
    auto ssp = make_shared<PII>(1, 2);
    printf("%d-%d\n", ssp->first, ssp->second);
    return 0;
}
