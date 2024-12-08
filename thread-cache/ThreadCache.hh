#include "common.hh"

class ThreadCache{
public:
    void* Allocate(size_t sz);//分配空间

    void Deallocate(void* obj, size_t sz);//返回空间


private:
    FreeList _free_lists[NFREELISTS]; 
};