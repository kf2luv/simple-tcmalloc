#pragma once
#include <iostream>
#include <cassert>
#include <mutex>

#if defined(_WIN32) || defined(__i386__) || defined(__ppc__)
typedef int PageID;
#elif defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__)
typedef unsigned long long PageID;
#endif

struct Span {
    Span* _next = nullptr;
    Span* _prev = nullptr;

    PageID _page_id = 0;  // 大块内存的起始页号
    size_t _npage = 0;    // 页数

    int _use_count = 0;   // 被使用的内存对象数
    FreeList _free_list;  // 存放内存对象的free链表
};

// 管理Span结构体的桶
// 带头双向链表
class SpanList {
   public:
    SpanList() {
        _dummy = new Span;
        _dummy->_next = _dummy;
        _dummy->_prev = _dummy;
    }

    void insert(Span* pos, Span* span) {
        assert(pos != nullptr);
        assert(span != nullptr);

        span->_next = pos->_next;
        span->_prev = pos;

        span->_next->_prev = span;
        span->_prev->_next = span;
    }

    void erase(Span* pos) {
        assert(pos != nullptr);
        assert(pos != _dummy);

        pos->_prev->_next = pos->_next;
        pos->_next->_prev = pos->_prev;
    }

    std::mutex& getMutex(){
        return _mtx;
    }

   private:
    Span* _dummy;     // 守卫节点
    std::mutex _mtx;  // 这个span链表的桶锁
};