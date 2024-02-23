#include <quiz/base.h>

#if 0
template<typename ...T>
class tuple;

template<typename First, typename... Rest>
class tuple : public tuple<Rest...>
{
public:
    tuple(First head, Rest... args) : tuple<Rest...>(args...), value(head)
    {
    }

    First value;
};

template<>
class tuple<> {};


template<int N, typename ...T>
struct tuple_at;

template<int N, typename First, typename... Rest>
struct tuple_at<N, tuple<First, Rest...> >
{
    static_assert(sizeof...(Rest) >= N, "wrong index");
    using value_type = typename tuple_at<N - 1, tuple<Rest...> >::value_type;
    using tuple_type = typename tuple_at<N - 1, tuple<Rest...> >::tuple_type;
};

template<typename First, typename ...Rest>
struct tuple_at<0, tuple<First, Rest...> >
{
    using value_type = First;
    using tuple_type = tuple<First, Rest...>;
};

template<int N, typename First, typename ...Rest>
decltype(auto) get(tuple<First, Rest...> tup)
{
    using value_type = typename tuple_at<N, tuple<First, Rest...> >::value_type;
    using tuple_type = typename tuple_at<N, tuple<First, Rest...> >::tuple_type;
    value_type ret = ((tuple_type) tup).value;
    return ret;
}
///////////////////////////////////////////////////////////////////////////
void test()
{
	tuple<int, double, const char *> test(12, 13.12, "123");

	auto ntest = get<0>(test);
	auto dtest = get<1>(test);
	auto csztest = get<2>(test);

	tuple<int> test2(22);
	auto ntest2 = get<0>(test2);
}

#endif
int main() {
}
