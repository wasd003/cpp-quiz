#include <memory>
#include <quiz/base.h>
#include <quiz/log.h>

template<typename T>
struct node {
    T data;
    std::shared_ptr<node<T>> next;
    std::weak_ptr<node<T>> prev;

    node(const T& data) : data(data) {}
};

template<typename T>
struct list {
    std::shared_ptr<node<T>> head;

    list() : head(std::make_shared<node<T>>(T{})) {
        head->next = head;
        head->prev = head;
    }

    void push_back(const T& data) {
        auto new_node = std::make_shared<node<T>>(data);
        std::shared_ptr<node<T>> back = head->prev.lock();
        back->next = new_node;
        new_node->prev = back;
        head->prev = new_node;
        new_node->next = head;
    }

    void pop_back() {
        auto back = head->prev.lock();
        if (back == head) return;
        auto sec_back = back->prev.lock();
        head->prev = sec_back;
        sec_back->next = head;
    }
};

int main() {
    list<int> l;
    auto print = [&l]() {
        auto cur = l.head;
        while (cur->next != l.head) {
            cur = cur->next;
            std::cout << cur->data;
        }
    };
    for (int i = 0; i < 5; i ++ ) l.push_back(i);
    print();
    std::cout << "\n";
    for (int i = 0; i < 2; i ++ ) l.pop_back();
    print();

}
