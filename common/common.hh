#pragma once
#include <iostream>
#include <cassert>
#include <mutex>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#endif

namespace cc_memory_pool
{
    // 获取bytes的最大值
    static const size_t MAX_MEM_SIZE = 256 * 1024;
    // ThreadCache从CentralCache拿取obj个数最大值 (32768)
    static const size_t MAX_FETCH_NUM = MAX_MEM_SIZE / 8;
    // 自由链表最大个数（thread cache中）
    static const size_t NFREELISTS = 208;
    // span链表最大个数 (central cache中)
    static const size_t NSPANLISTS = 208;
    // page链表最大个数 (page cache中)
    static const size_t NPAGELISTS = 128;
    // 规定一页8KB (8KB = 2^13B)
    static const size_t PAGE_SHIFT = 13;

    // 向系统申请内存
    inline void *systemAlloc(size_t kpage)
    {
        void *ptr = nullptr;

#if defined(_WIN32) || defined(_WIN64)
        ptr = VirtualAlloc(0, kpage << PAGE_SHIFT, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else
        ptr = mmap(NULL, kpage << PAGE_SHIFT, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#endif

        if (ptr == nullptr)
        {
            throw std::bad_alloc();
        }

        return ptr;
    }
}

#include "SizeClass.hh"
#include "FreeList.hh"
#include "Span.hh"