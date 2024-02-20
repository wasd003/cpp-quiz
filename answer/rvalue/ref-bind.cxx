#include <quiz/base.h>

void lvalue_ref(std::vector<int>&) {
    std::cout << "lvalue ref" << std::endl;
}

/**
 * 左值引用可以绑定各种类型的左值表达式，包括右值引用类型
 */
void lvalue_ref_demo() {
    std::vector<int>&& v1  = {1, 2, 3};
    std::vector<int>v2  = {1, 2, 3};
    std::vector<int>& v3  = v2;
    lvalue_ref(v1);
    lvalue_ref(v2);
    lvalue_ref(v3);
}

void const_lvalue_ref(const std::vector<int>&) {
    std::cout << "const lvalue ref" << std::endl;
}

/**
 * 常量左值引用既可以绑定左值表达式也可以绑定右值表达式
 */
void const_lvalue_ref_demo() {
    // lvalue
    std::vector<int>&& v1  = {1, 2, 3};
    std::vector<int>v2  = {1, 2, 3};
    std::vector<int>& v3  = v2;
    const_lvalue_ref(v1);
    const_lvalue_ref(v2);
    const_lvalue_ref(v3);
    // rvalue
    const_lvalue_ref({1, 2, 3});
}

void rvalue_ref(std::vector<int>&&) {
    std::cout << "rvalue ref" << std::endl;
}

/**
 * 右值引用只能绑定右值表达式，且类型必须是T或者T&&
 */
void rvalue_ref_demo() {
    std::vector v = {1, 2, 3};
    rvalue_ref(static_cast<std::vector<int>>(v));
    rvalue_ref(static_cast<std::vector<int>&&>(v));
    rvalue_ref({1, 2, 3});
}

int main() {
    lvalue_ref_demo();

    const_lvalue_ref_demo();

    rvalue_ref_demo();
}
