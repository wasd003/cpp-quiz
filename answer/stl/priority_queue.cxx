#include <cassert>
#include <quiz/base.h>

template<typename T>
struct less {
    bool operator()(const T& lhs, const T& rhs) const {
        return lhs < rhs;
    }
};

template<typename T, typename C = std::vector<T>, typename F = less<T>>
struct priority_queue {
private:
    C container;
    F f;

public:
    priority_queue() : container(1, T{}), f(F{}) {}

    priority_queue(F f) : container(1, T{}), f(f) {}

    void push(const T& data) {
        container.push_back(data);
        int cur = container.size() - 1;
        while (cur > 1) {
            int fa = cur / 2;
            if (f(container[fa], container[cur])) {
                std::swap(container[fa], container[cur]);
                cur = fa;
            } else break;
        }
    }

    void pop() {
        std::swap(container[1], container.back());
        container.pop_back();
        size_t cur = 1;
        while (cur < container.size()) {
            auto ls = cur * 2, rs = cur * 2 + 1;
            auto best = container[cur];
            auto best_idx = cur;
            if (ls < container.size() && f(best, container[ls])) {
                best = container[ls];
                best_idx = ls;
            }

            if (rs < container.size() && f(best, container[rs])) {
                best = container[rs];
                best_idx = rs;
            }
            if (best_idx == cur) break;
            std::swap(container[cur], container[best_idx]);
            cur = best_idx;
        }
    }

    T& top() {
        return container[1];
    }

    size_t size() const {
        return container.size() - 1;
    }
};

int main() {
    priority_queue<int> heap;
    std::priority_queue<int> ans_heap;
    std::vector<int> data {2, 5, 4,3,2,3,4,0,-1,2};
    for (auto x : data) {
        heap.push(x);
        ans_heap.push(x);
    }
    while (heap.size()) {
        auto cur = heap.top();
        heap.pop();
        auto ans = ans_heap.top();
        ans_heap.pop();
        assert(cur == ans);
    }
    return 0;
}
