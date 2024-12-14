#include "ThreadCache.hh"

using namespace cc_memory_pool;

void *ThreadCache::allocate(size_t bytes)
{
    assert(bytes > 0);

    // 根据对齐策略，选择对应的free_list
    size_t idx = SizeClass::index(bytes);
    FreeList &freeList = _freeLists[idx];

    if (freeList.empty())
    {
        // free_list中无内存对象，先去CentralCache拿到free_list中
        fetchObjFromCentralCache(bytes, freeList);
    }
    return freeList.pop();
}

void ThreadCache::deallocate(void *obj, size_t bytes)
{
    assert(obj != nullptr);
    assert(bytes > 0);

    // 根据对齐策略，选择对应的free_list
    size_t idx = SizeClass::index(bytes);
    FreeList &free_list = _freeLists[idx];

    free_list.push(obj);
}

// threadCache从centrealCache中批量获取内存对象
// bytes: 欲获取的内存对象的大小
// free_list：获取到的内存对象，统一放到这里面

void ThreadCache::fetchObjFromCentralCache(size_t bytes, FreeList &freeList)
{
    // 一次从CentralCache拿多少个obj?
    // 太少，频繁去拿，锁竞争问题
    // 太多，用不完，浪费，别的线程还要用

    // 计算阈值
    size_t threshold = SizeClass::numFetchObj(bytes);
    // 慢开始算法
    size_t fetchNum = 0;
    if (freeList.maxFetchNum() < threshold)
    {
        freeList.maxFetchNum() += 1;
        fetchNum = freeList.maxFetchNum();
    }
    else
    {
        fetchNum = threshold;
    }
    // 获取CentralCache对象
    void *begin = nullptr;
    void *end = nullptr;

    size_t actualNum = CentralCache::getInstance()->getRangeObj(begin, end, fetchNum, bytes);
    assert(actualNum > 0);

    freeList.pushRange(begin, end);
}