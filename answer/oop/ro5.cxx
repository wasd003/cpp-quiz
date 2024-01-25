#include <cassert>
#include <exception>
#include <memory>
#include <quiz/base.h>
#include <quiz/log.h>
#include <fcntl.h>
#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>
#include <tuple>
#include <unistd.h>

template<typename T>
class base {
private:
    std::string file_path;
    size_t count {0};
    int file_fd {-1};
    T *ptr {nullptr};

private:
    void cleanup() {
        delete[] ptr;
        if (file_fd != -1) close(file_fd);
    }

    void init() {
        try {
            file_fd = open(file_path.c_str(), O_RDONLY);
            if (file_fd == -1) throw std::runtime_error("file not exist");
            ptr = new T[count];
        } catch (const std::exception& e) {
            cleanup();
            throw std::runtime_error("ctor failed:" + std::string(e.what()));
        }
    }

public:
    base(const std::string& file_path, size_t count) :
        file_path(file_path), count(count) {
        init();
    }

    T& operator[](size_t index) {
        return ptr[index];
    }

    base(const base& rhs) : file_path(rhs.file_path), count(rhs.count) {
        init();
        std::copy(rhs.ptr, rhs.ptr + count, ptr);
    }

    base& operator=(const base& rhs) {
        auto tmp = rhs;
        swap(tmp);
        return *this;
    }

    base(base&& rhs) :
        file_path(std::move(rhs.file_path)),
        count(rhs.count), file_fd(rhs.file_fd),
        ptr(rhs.ptr) {
        rhs.file_fd = -1;
        rhs.ptr = nullptr;
        rhs.count = 0;
    }

    base& operator=(base&& rhs) {
        auto tmp = std::move(rhs);
        swap(tmp);
        return *this;
    }

    virtual ~base() {
        cleanup();
    }

    void swap(base& rhs) noexcept {
        file_path.swap(file_path);
        std::swap(count, rhs.count);
        std::swap(file_fd, rhs.file_fd);
        std::swap(ptr, rhs.ptr);
    }
};

class file_raii {
private:
    int file_fd;

public:
    file_raii(const std::string& file_path) {
        file_fd = open(file_path.c_str(), O_RDONLY);
        if (file_fd == -1) throw std::runtime_error("file not exist");
    }

    void swap(file_raii& rhs) noexcept {
        std::swap(file_fd, rhs.file_fd);
    }

    ~file_raii() {
        if (file_fd != -1) close(file_fd);
    }

    file_raii(const file_raii&) = delete;
    file_raii& operator=(const file_raii&) = delete;
    file_raii(file_raii&&) = default;
    file_raii& operator=(file_raii&&) = default;
};

template<typename T>
class derived : public base<T> {
private:
    std::unique_ptr<T> up;
    file_raii fd;

public:
    derived(const std::string& base_file_path, size_t base_count,
            const std::string& derived_file_path, size_t derived_count) :
        base<T>(base_file_path, base_count),
        up(std::make_unique<T>(derived_count)),fd(derived_file_path) {}

    derived(const derived& rhs) = delete;

    derived& operator=(const derived& rhs) = delete;
    // derived is raii class, actually no need to implement ro5 explicitly
    // here only implement move operation to demo
    derived(derived&& rhs) : base<T>(std::move(rhs)),
                             up(std::move(rhs.up)),
                             fd(rhs.fd) {}

    derived& operator=(derived&& rhs) {
        auto tmp = std::move(rhs);
        swap(tmp);
        return *this;
    }

    ~derived() override = default;

    void swap(derived& rhs) noexcept {
        base<T>::swap(rhs);
        up.swap(rhs.up);
        fd.swap(rhs.fd);
    }
};

int main() {
    const size_t count = 1024;
    const std::string valid_file_path = "/home/jch/test.txt";
    const std::string invalid_file_path = "blahblahblah";
    { // base test
        {
            // test failed ctor
            try {
                base<int> b {invalid_file_path, count};
            } catch(const std::exception& e) {
                debug(e.what());
            }
        }
        {
            // test successful ctor
            base<int> b {valid_file_path, count};
            for (size_t i = 0; i < count; i ++ ) b[i] = i;
            for (size_t i = 0; i < count; i ++ ) assert(b[i] == static_cast<int>(i));
        }
        {
            // test copy ctor && copy assignment operator
            base<int> b {valid_file_path, count};
            for (size_t i = 0; i < count; i ++ ) b[i] = i;
            auto clone = b;
            for (size_t i = 0; i < count; i ++ ) assert(clone[i] == static_cast<int>(i));
            auto obj = b;
            obj[0] = 12;
            assert(obj[0] == 12);
            obj = clone;
            assert(obj[0] == 0);
        }
        {
            // test move ctor && move assignment operator
            base<int> b {valid_file_path, count};
            for (size_t i = 0; i < count; i ++ ) b[i] = i;
            auto m = std::move(b);
            for (size_t i = 0; i < count; i ++ ) assert(m[i] == static_cast<int>(i));
            auto o = m;
            o[0] = 12;
            assert(o[0] == 12);
            o = std::move(m);
            assert(o[0] == 0);
        }
    }
    { 
        // TODO: test derived
    }

    debug("pass ", __FILE__, " test");
    return 0;
}
