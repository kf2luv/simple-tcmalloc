#include <thread>

#include "concurrentAlloc.hh"

void alloc_test_1() {
    for (int i = 0; i < 10; i++) {
        void* ptr = ccAlloc(5);
    }
}

void alloc_test_2() {
    for (int i = 0; i < 10; i++) {
        void* ptr = ccAlloc(8);
    }
}

void alloc_test() {
    std::thread t1(alloc_test_1);
    std::thread t2(alloc_test_2);

    t1.join();
    t2.join();
}

int main() {
    // alloc_test();
    return 0;
}