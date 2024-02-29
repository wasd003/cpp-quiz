#include <alloca.h>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <list>
#include <quiz/base.h>

template<typename T>
struct object_pool {
    const static size_t PAGE_SIZE {32};
    const static size_t OSIZE {sizeof (T)};
    std::list<uint8_t*> page_list;
    uint8_t* freelist_head {nullptr};
    uint16_t cur_last {0};

    #define NEXT_PTR(p) (reinterpret_cast<uint8_t**>(p))

    object_pool() {
        assert(PAGE_SIZE % OSIZE == 0);
        assert(OSIZE >= sizeof (uint8_t*)); // at least store a pointer
        page_list.emplace_back(new uint8_t [PAGE_SIZE]);
    }

    uint8_t *allocate() {
    reallocate:
        printf("allocate: ");
        if (cur_last < PAGE_SIZE) {
            printf("allocate in page:%p\n", page_list.front());
            auto ans = page_list.front() + cur_last;
            cur_last += OSIZE;
            return ans;
        } else if (freelist_head) {
            uint8_t *ans = freelist_head;
            printf("allocate in freelist:%p\n", ans);
            freelist_head = *NEXT_PTR(freelist_head);
            return ans;
        } else {
            uint8_t *new_page = new uint8_t[PAGE_SIZE];
            printf("new page:%p\n", new_page);
            page_list.push_front(new_page);
            cur_last = 0;
            goto reallocate;
        }
    }

    void destroy(uint8_t *p) {
        printf("return object:%p to freelist\n", p);
        *NEXT_PTR(p) = freelist_head;
        freelist_head = p;
    }

    ~object_pool() {
        for (auto x : page_list) delete [] x;
    }
};

int main() {
    using PII = std::pair<int, int>;
    object_pool<PII> opool;
    auto p = opool.allocate();
    opool.destroy(p);
    std::vector<uint8_t*> ptr_list;
    for (int i = 0; i < 10; i ++ ) {
        p = opool.allocate();
        ptr_list.push_back(p);
    }
    for (auto x : ptr_list) opool.destroy(x);
    return 0;
}
