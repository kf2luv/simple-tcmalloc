#include "PageCache.hh"

PageCache *PageCache::getInstance()
{
    return &_instance;
}

//从PageCache申请一个存有k页的span
Span *PageCache::newSpan(size_t k)
{
    assert(k > 0 && k <= NPAGELISTS);

    std::unique_lock<std::mutex> lockguart(_mtx);

    SpanList &spanList = _spanLists[k];

    if (!spanList.empty())
    {
        return spanList.popFront();
    }
    else
    {
        // 遍历后面的页链表，如果后面存在更大page的span，可以对其进行拆分
        for (size_t i = k + 1; i <= NPAGELISTS; i++)
        {
            if (!_spanLists[i].empty())
            {
                // 从_spanLists[i]中取一个span，并对其中的大块内存进行拆分
                // 从头切k页下来，剩下的挂到对应映射的位置

                Span* nSpan = _spanLists[i].popFront();
                Span* kSpan = new Span;

                kSpan->_pageID = nSpan->_pageID;
                kSpan->_npage = k;

                nSpan->_pageID += k;
                nSpan->_npage -= k;

                _spanLists[nSpan->_npage].pushFront(nSpan);
                return kSpan;
            }
        }
    }

    // 走到这里代表Page Cache内不存在有效的span，需要向系统（堆）申请
    // TODO
}

PageCache PageCache::_instance;