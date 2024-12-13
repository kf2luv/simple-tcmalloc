#pragma once
#include "../common/common.hh"
#include "../central-cache/CentralCache.hh"

class ThreadCache {
   public:
    // 分配空间
    void* allocate(size_t bytes);
    // 返回空间
    void deallocate(void* obj, size_t bytes);

   private:
    // 从CentralCache中拿到对应字节数的内存对象
    void fetchObjFromCentralCache(size_t bytes, FreeList& freeList);

   private:
    FreeList _freeLists[NFREELISTS];
};