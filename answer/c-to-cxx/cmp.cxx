#include <quiz/base.h>
#include <quiz/log.h>
#include <array>

template<auto N>
consteval decltype(auto) get_primes() {
    std::array<bool, N + 1> is_primes;
    is_primes.fill(true);
    is_primes[1] = false;
    for (int i = 2; i <= N; i ++ ) {
        if (!is_primes[i]) continue;
        for (int j = i + i; j <= N; j += i)
            is_primes[j] = false;
    }
    return is_primes;
}

int main() {
    auto is_primes = get_primes<100>();
    UNUSED(is_primes);
    for (size_t i = 0; i < is_primes.size(); i ++ )
        debug(i, ":", is_primes[i]);

    return 0;
}
