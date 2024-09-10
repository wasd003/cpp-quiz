## 题目描述
catch异常和函数传参有什么区别？
1. 没有隐式类型转换
2. first fit vs best fit
3. throw出去的对象总是要拷贝一份，但是函数返回值有RVO
