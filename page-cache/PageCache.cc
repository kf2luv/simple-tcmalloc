#include "PageCache.hh"

cc_memory_pool::PageCache cc_memory_pool::PageCache::_instance;

cc_memory_pool::PageCache* cc_memory_pool::PageCache::getInstance()
{
	return &_instance;
}

// 从PageCache申请一个存有k页的span
cc_memory_pool::Span* cc_memory_pool::PageCache::newSpan(size_t k)
{
	assert(k > 0 && k <= NPAGELISTS);

	// std::unique_lock<std::mutex> lockguard(_mtx);

	SpanList& spanList = _spanLists[k];

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

				Span* nSpan = _spanLists[i].popFront();//找到的大空间nSpan
				Span* kSpan = new Span;//nSpan前k个页切出来的kSpan

				kSpan->_pageID = nSpan->_pageID;
				kSpan->_npage = k;
				//将切分出来的空间挂载到kSpan的free链表中
				kSpan->_freeList.head() = nSpan->_freeList.head();

				nSpan->_pageID += k;
				nSpan->_npage -= k;
				//nSpan的空间往后移动k页
				nSpan->_freeList.head() = (void*)(nSpan->_pageID << PAGE_SHIFT);

				_spanLists[nSpan->_npage].pushFront(nSpan);
				return kSpan;
			}
		}
	}

	// 走到这里代表Page Cache内不存在有效的span，需要向系统（堆）申请一个128page的空间（后续方便被拆分）
	void* memory = systemAlloc(NPAGELISTS);
	assert(memory);

	Span* greatSpan = new Span;
	//修改span的空间，页数和页号
	greatSpan->_freeList = FreeList(memory);
	greatSpan->_npage = NPAGELISTS;

	uintptr_t address = reinterpret_cast<uintptr_t>(memory);
	greatSpan->_pageID = address >> PAGE_SHIFT;


	_spanLists[NPAGELISTS].pushFront(greatSpan);
	// 此时只是新增了大Span，还需要拆分后返回给用户，复用代码
	return newSpan(k);
}

