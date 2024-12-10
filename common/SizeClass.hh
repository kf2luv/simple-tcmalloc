#pragma once
#include <iostream>
#include <cassert>

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

    // 计算ThreadCache从CentralCache拿取obj个数的阈值
    static size_t fetchObjNumThreshold(size_t bytes) {
        assert(bytes > 0);
        size_t threshold = MAX_FETCH_NUM / bytes;  // obj小，阈值大;obj大，阈值小

        if (threshold < 2) {  // 下限
            threshold = 2;
        } else if (threshold > 512) {  // 上限
            threshold = 512;
        }
        return threshold;
    }

   private:
    static inline size_t _roundUp(size_t bytes, size_t align_num) {
        return ((bytes + align_num - 1) & ~(align_num - 1));
    }
    static inline size_t _index(size_t bytes, size_t align_shift) {
        return ((bytes + (1 << align_shift) - 1) >> align_shift) - 1;
    }
};