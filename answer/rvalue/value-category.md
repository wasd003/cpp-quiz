值类别是左值/右值，具体可以分为lvalue/rvalue/prvalue/xvalue/glvalue
类型是T/const T/const T&/T&&这些
这是两个正交的概念，比如左值引用可以引用匹配类型的所有左值表达式，即便类型是右值引用
