#include <thread>
#include "ccAlloc.hh"

void alloc_test_1()
{
    for (int i = 0; i < 10; i++)
    {
        void *ptr = cc_memory_pool::ccAlloc(5);
    }
}

void alloc_test_2()
{
    for (int i = 0; i < 10; i++)
    {
        void *ptr = cc_memory_pool::ccAlloc(8);
    }
}

void alloc_test()
{
    std::thread t1(alloc_test_1);
    std::thread t2(alloc_test_2);

    t1.join();
    t2.join();
}

void alloc_test1()
{
    for (int i = 0; i < 10; i++)
    {
        int *ptr = (int *)cc_memory_pool::ccAlloc(sizeof(int));
        *ptr = i + 1;
        printf("%p: %d\n", ptr, *ptr);
    }
}

int main()
{
    alloc_test1();
    return 0;
}