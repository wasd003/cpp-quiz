#include <cstddef>
#include <quiz/base.h>

template<typename T>
struct mini_iterator_traits {
    using iterator_category = typename T::iterator_category;
    using value_type = typename T::value_type;
    using difference_type = typename T::difference_type;
};

template<typename T>
struct mini_iterator_traits<T*> {
    using iteator_category = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
};

template<typename T>
struct mini_iterator_traits<const T*> {
    using iteator_category = std::random_access_iterator_tag;
    using value_type = const T;
    using difference_type = std::ptrdiff_t;
};

int main() {
    std::vector vec {1, 2, 3};
    auto it = vec.begin();
    using value_type = typename mini_iterator_traits<decltype(it)>::value_type;
    static_assert(std::is_same_v<int, value_type>);

    uint64_t *ptr = nullptr;
    using sec_value_type = typename mini_iterator_traits<decltype(ptr)>::value_type;
    static_assert(std::is_same_v<uint64_t, sec_value_type>);
    return 0;
}
