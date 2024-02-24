## 题目描述
使用自定义类型作为`unordered_map`的key

提示：自定义类型需要满足两个条件：
1. 可哈希（特化std::hash模板）
2. 可比较是否相等（重载operator==）
