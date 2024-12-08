#include <cassert>
#include <iostream>

static const size_t MAX_MEM_SIZE = 256 * 1024;
static const size_t NFREELISTS = 208;

class FreeList {
   public:
    void push(void* obj) {  // 头插
        assert(obj != nullptr);
        nextObj(obj) = _free_list;
        _free_list = obj;
    }
    void* pop() {
        if(_free_list == nullptr){
            return nullptr;
        }
        void* ret = _free_list;
        _free_list = nextObj(_free_list);
        return ret;
    }
    bool empty(){
        return _free_list == nullptr;
    }

   private:
    void*& nextObj(void* obj) {  // 获取内存对象的下一个对象的地址
        return *(void**)obj;
    }

    void* _free_list;
};

class SizeClass {
   public:
    // 接受一个大小bytes，返回其向上调整后的结果
    static size_t roundUp(size_t bytes) {
        assert(bytes > 0 && bytes <= MAX_MEM_SIZE);
        if (bytes <= 128) {
            return _roundUp(bytes, 8);

        } else if (bytes <= 1024) {
            return _roundUp(bytes, 16);

        } else if (bytes <= 8 * 1024) {
            return _roundUp(bytes, 128);

        } else if (bytes <= 64 * 1024) {
            return _roundUp(bytes, 1024);

        } else {
            return _roundUp(bytes, 8192);
        }
    }

    // 接受一个大小bytes，返回对应free链表的下标
    static size_t index(size_t bytes) {
        assert(bytes > 0 && bytes <= MAX_MEM_SIZE);
        int group[4] = {16, 56, 56, 56};

        if (bytes <= 128) {
            return _index(bytes, 3);
        } else if (bytes <= 1024) {
            return _index(bytes - 128, 4) + group[0];
        } else if (bytes <= 8 * 1024) {
            return _index(bytes - 1024, 7) + group[0] + group[1];
        } else if (bytes <= 64 * 1024) {
            return _index(bytes - 8 * 1024, 10) + group[0] + group[1] +
                   group[2];

        } else {
            return _index(bytes - 64 * 1024, 13) + group[0] + group[1] +
                   group[2] + group[3];
        }
    }

   private:
    static inline size_t _roundUp(size_t bytes, size_t align_num) {
        if (bytes % align_num == 0) {
            return bytes;
        }
        return bytes - bytes % align_num + align_num;
    }
    static inline size_t _index(size_t bytes, size_t align_shift) {
        return ((bytes + (1 << align_shift) - 1) >> align_shift) - 1;
    }
};