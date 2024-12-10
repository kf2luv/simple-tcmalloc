#include "ThreadCache.hh"

void* ThreadCache::allocate(size_t bytes) {
    assert(bytes > 0);

    // 根据对齐策略，选择对应的free_list
    size_t idx = SizeClass::index(bytes);
    FreeList& free_list = _free_lists[idx];

    if (free_list.empty()) {
        // free_list中无内存对象，先去CentralCache拿
        fetchObjFromCentralCache(bytes, free_list);
    }
    return free_list.pop();

    return nullptr;
}

void ThreadCache::deallocate(void* obj, size_t bytes) {
    assert(obj != nullptr);
    assert(bytes > 0);

    // 根据对齐策略，选择对应的free_list
    size_t idx = SizeClass::index(bytes);
    FreeList& free_list = _free_lists[idx];

    free_list.push(obj);
}

void ThreadCache::fetchObjFromCentralCache(size_t bytes, FreeList& free_list) {
    // 一次从CentralCache拿多少个?
    // 太少，频繁去拿，锁竞争问题
    // 太多，用不完，浪费，别的线程还要用

    // 计算阈值
    size_t threshold = SizeClass::fetchObjNumThreshold(bytes);
    // 慢开始算法
    size_t fetchNum = 0;
    if (free_list.maxFetchNum() < threshold) {
        free_list.maxFetchNum() += 1;
        fetchNum = free_list.maxFetchNum();
    } else {
        fetchNum = threshold;
    }
    //获取CentralCache对象
    void* begin = nullptr;
    void* end = nullptr;
    
    size_t actualNum = CentralCache::getInstance()->getRangeObj(begin, end, fetchNum, bytes);
    assert(actualNum > 0);
    free_list.pushRange(begin, end);
}