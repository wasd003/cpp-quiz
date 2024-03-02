#include <alloca.h>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <list>
#include <quiz/base.h>

template<typename T>
struct kmem_cache {
private:
    const static uint16_t base = sizeof (uint8_t**);
    const static uint16_t PAGE_SIZE = 4096U;
    const static uint16_t ObjectSize = sizeof (T);
    uint8_t *page_head {nullptr};
    uint8_t *freelist_head {nullptr};
    uint16_t offset {base};

    #define NEXT_PTR(x) (reinterpret_cast<uint8_t**>(x))

public:
    kmem_cache() {
        page_head = new uint8_t[PAGE_SIZE];
        *NEXT_PTR(page_head) = nullptr;
    }

    ~kmem_cache() {
        auto page = page_head;
        while (page) {
            auto next = *NEXT_PTR(page);
            delete[] page;
            page = next;
        }
    }

    kmem_cache(const kmem_cache&) = delete;
    kmem_cache& operator=(const kmem_cache&) = delete;
    kmem_cache(kmem_cache&&) = delete;
    kmem_cache& operator=(kmem_cache&&) = delete;

    T *allocate() {
    reallocate:
        if (offset + ObjectSize < PAGE_SIZE) { // case #1: allocate in current page
            auto res = page_head + offset;
            offset += ObjectSize;
            return reinterpret_cast<T*>(res);
        } else if (freelist_head) { // case #2: allcocate from freelist
            auto res = freelist_head;
            freelist_head = *NEXT_PTR(freelist_head);
            return reinterpret_cast<T*>(res);
        } else { // case #3: allocate new page
            auto new_page = new uint8_t [PAGE_SIZE];
            *NEXT_PTR(new_page) = page_head;
            page_head = new_page;
            offset = base;
            goto reallocate;
        }
    }

    void destroy(T *obj) {
        uint8_t *p = reinterpret_cast<uint8_t *>(obj);
        *NEXT_PTR(p) = freelist_head;
        freelist_head = p;
    }
};

int main() {
    using PII = std::pair<int, int>;
    kmem_cache<PII> opool;
    auto p = opool.allocate();
    opool.destroy(p);
    std::vector<PII*> ptr_list;
    for (int i = 0; i < 10; i ++ ) {
        p = opool.allocate();
        ptr_list.push_back(p);
    }
    for (auto x : ptr_list) opool.destroy(x);
    return 0;
}
