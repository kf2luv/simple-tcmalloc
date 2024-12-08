#include "ThreadCache.hh"

void* ThreadCache::Allocate(size_t bytes) {
    // 根据对齐策略，选择对应的free_list
    size_t idx = SizeClass::index(bytes);
    FreeList& free_list = _free_lists[idx];

    if (!free_list.empty()) {
        return free_list.pop();
    } else {
        // free_list中无内存对象，再去CentralCache拿
        // TODO
    }
}

void ThreadCache::Deallocate(void* obj, size_t bytes) {
    // 根据对齐策略，选择对应的free_list
    size_t idx = SizeClass::index(bytes);
    FreeList& free_list = _free_lists[idx];

    free_list.push(obj);
}
