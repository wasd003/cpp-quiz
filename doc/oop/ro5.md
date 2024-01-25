## 题目描述
先编写一个RAII类，同时管理一个内存指针以及一个文件fd。然后再编写一个派生类，管理另一个内存指针和文件fd。

## 题目要求
- 内存指针的类型由模板参数给出
- 正确处理ctor构造失败的情况
    - 基类采用try-catch的方式处理
    - 派生类采用raii的方式处理
- assignment operator应考虑以下case：
    - 是否正确释放了原有资源
    - 自我赋值
    - 实现原子级别异常安全
    

## 目的
这道题主要练习同时涉及继承以及模板的时候，如何正确编写ctor/dtor/assignment operator。

## 思考
答案中的assignment operator使用copy-and-swap实现，这样做有什么缺点？