#include <quiz/base.h>

template<typename T>
struct function {};

template<typename R, typename... Args>
struct function<R(Args...)> {
private:
    struct function_interface {
        virtual R call(Args&&... args) = 0;

        virtual ~function_interface() {}
    };

    template<typename F>
    struct function_impl : function_interface {
        F f;
        virtual R call(Args&&... args) override {
            return f(std::forward<Args>(args)...);
        }

        function_impl(F _f) : f(_f) {}

        virtual ~function_impl() {}
    };

    std::unique_ptr<function_interface> pimpl;

public:
    template<typename F>
    function(F f) : pimpl(std::make_unique<function_impl<F>> (std::move(f))) {}

    R operator()(Args&&... args) {
        assert(pimpl);
        return pimpl->call(std::forward<Args>(args)...);
    }
};

int main() {
    function<int(int, int)> f = [](int a, int b) { return a + b; };

    auto ans = f(3, 4);
    std::cout << ans << std::endl;
}
