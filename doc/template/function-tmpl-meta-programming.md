## 概述
一共3道题，这组题主要考察函数模板元编程，用以实现编译期多态。以下题目如果没有特殊说明，同时使用concept和非concept两种做法实现。

## T1
实现一个函数，对于trivial类型表现出一种行为，对于非trivial表现出另一种行为。
## T2
实现一个函数，参数为迭代器。根据迭代器类型表现出不同行为。比如`random_access`一种行为，`bidirectional`一种行为。
- tag dispatch
- if constexpr
- concept
## T3
实现一个函数，对于存在名为`ok(int, int)`成员函数的类型表现出一种行为，反之表现出另一种行为。
