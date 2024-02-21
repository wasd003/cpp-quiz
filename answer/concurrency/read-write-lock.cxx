#include <quiz/base.h>
#include <thread>
#include <shared_mutex>
#include <mutex>

constinit std::shared_mutex mtx;
int cnt = 0;

int main() {
    std::thread reader([]() {
        for (;;) {
            std::shared_lock lk(mtx);
            std::cout << cnt << std::endl;
        }
    });

    std::thread writer([](){
        for (;;) {
            std::unique_lock lk(mtx);
            cnt ++ ;
        }
    });

    reader.join();
    writer.join();
    return 0;
}
