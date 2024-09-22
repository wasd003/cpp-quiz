atomic要求instantiate的类型满足如下条件：
- trivial copyable
- trivial destructable

对于T的大小并无限制。但如果T过大，比如超出8个byte，那么对于其上的原子操作，就不能再保证无锁了。
究竟实例化出来的atomic<T>是否真的无锁，可以通过is_lock_free()这个函数检查.
