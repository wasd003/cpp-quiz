#include <cassert>
#include <quiz/base.h>
#include <type_traits>
#include <utility>

template<typename T, size_t Capacity = 2>
struct vector {
private:
    T* begin;
    T* end;
    T* last;

    bool full() {
        return last == end;
    }

    void grow() {
        auto old_begin = begin;
        auto old_last = last;
        auto capacity = (end - begin) * 2;

        begin = new T[capacity];
        end = begin + capacity;
        last = begin;
        for (auto i = old_begin; i != old_last; i ++ , last ++ ) {
            *last = std::move_if_noexcept(*i);
        }
        assert(last - begin == old_last - old_begin);
        delete[] old_begin;
    }

    void destroy_elements_aux() {
        for (auto i = begin; i != last; i ++ ) {
            i->~T();
        }
    }
#if 1
    // Method #1: SFINAE
    template<typename Q = T>
    std::enable_if_t<std::is_trivial_v<Q>> destroy_elements() {}

    template<typename Q = T>
    std::enable_if_t<!std::is_trivial_v<Q>> destroy_elements() {
        destroy_elements_aux();
    }
#elif 1
    // Method #2: C++17 Way
    void destroy_elements_trivial() {}

    void destroy_elements_nontrivial() {
        destroy_elements_aux();
    }

    void destroy_elements() {
        if constexpr (std::is_trivial_v<T>) {
            destroy_elements_trivial();
        } else {
            destroy_elements_nontrivial();
        }
    }
#elif 1
    // Method #3: C++20 Way
    template<typename Q = T>
    requires std::is_trivial_v<Q>
    void destroy_elements() {}

    template<typename Q = T>
    void destroy_elements() {
        destroy_elements_aux();
    }
#elif 1
    // Method #4: tricky way
    template<typename Q, typename = void>
    struct destroy_elements_wrapper {
        static void destroy_elements(vector<T>& v) {
            v.destroy_elements_aux();
        }
    };
    template<typename Q>
    struct destroy_elements_wrapper<Q, std::enable_if_t<std::is_trivial_v<Q>>> {
        static void destroy_elements(vector<T>& v) {}
    };

    void destroy_elements() {
        destroy_elements_wrapper<T>::destroy_elements(*this);
    }
#endif

public:
    vector() : begin(new T[Capacity]), end(begin + Capacity), last(begin) {}

    size_t size() const {
        return last - begin;
    }

    size_t capacity() const {
        return end - begin;
    }

    void swap(vector& rhs) noexcept {
        std::swap(begin, rhs.begin);
        std::swap(end, rhs.end);
        std::swap(last, rhs.last);
    }

    void push_back(const T& data) {
        if (full()) grow();
        *(last ++ ) = data;
    }

    void push_back(T&& data) {
        if (full()) grow();
        *(last ++ ) = std::move(data);
    }

    template<typename... Args>
    void emplace_back(Args&&... args) {
        if (full()) grow();
        new(last ++ ) T(std::forward<Args>(args)...);
    }

    void pop_back() {
        assert(size());
        last->~T();
        last -- ;
    }

    vector(const vector& rhs) :
                begin(new T[rhs.capacity()]),
                end(begin + rhs.capacity()),
                last(begin + rhs.size()) {
        for (auto i = begin, j = rhs.begin; j != rhs.last; i ++ ,j ++ ) {
            *i = *j;
        }
    }

    vector& operator=(const vector& rhs) {
        if (rhs.size() <= capacity()) { // reuse current memory
            destroy_elements();
            for (auto i = begin, j = rhs.begin; j != rhs.last; i ++ ,j ++ ) {
                *i = *j;
            }
            last = begin + rhs.size();
        } else { // use new memory
            auto tmp = rhs;
            swap(tmp);
        }
        return *this;
    }

    vector(vector&& rhs) : begin(rhs.begin), end(rhs.end), last(rhs.last) {
        rhs.begin = rhs.end = rhs.last = nullptr;
    }

    vector& operator=(vector&& rhs) {
        auto tmp = std::move(rhs);
        swap(tmp);
        return *this;
    }
    
    ~vector() {
        destroy_elements();
        delete[] begin;
        begin = end = last = nullptr;
    }
    
    T& operator[](int idx) {
        return begin[idx];
    }
};

auto print = []<typename T>(T&& vec) {
    printf("vec:");
    for (size_t i = 0; i < vec.size(); i ++ ) {
        auto [x, y] = vec[i];
        printf("[%d,%d] ", x, y);
    }
    printf("\n");
};

void test_single_vector() {
    vector<std::pair<int, int>> vec;
    const int n = 5;
    for (int i = 0; i < n; i ++ ) {
        vec.emplace_back(i, i);
    }
    print(vec);

    for (int i = 0; i < n; i ++ ) {
        vec.push_back({i + n, i + n});
    }
    print(vec);
    while (vec.size()) vec.pop_back();
    print(vec);
}

void test_multi_vector() {
    vector<std::pair<int, int>> vec;
    const int n = 5;
    for (int i = 0; i < n; i ++ ) vec.emplace_back(i, i);
    // copy ctor
    auto copy_vec = vec;
    print(copy_vec);

    // move ctor
    auto move_vec = std::move(copy_vec);
    print(move_vec);
    assert(!copy_vec.size());

    // copy =
    copy_vec = vec;
    print(copy_vec);

    // move =
    for (int i = 0; i < n; i ++ ) vec.emplace_back(i + n, i + n);
    move_vec = std::move(vec);
    print(move_vec);
}

int main() {
    test_single_vector();

    test_multi_vector();
    return 0;
}
