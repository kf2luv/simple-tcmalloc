#include "CentralCache.hh"

cc_memory_pool::CentralCache cc_memory_pool::CentralCache::_instance;

cc_memory_pool::CentralCache *cc_memory_pool::CentralCache::getInstance()
{
	return &_instance;
}

// 获取一个有效的span
cc_memory_pool::Span *cc_memory_pool::CentralCache::getOneEffectiveSpan(SpanList &spanList, size_t bytes)
{
	// 1.在Central Cache中映射的span_list中，查找是否存在有效Span
	Span *begin = spanList.begin();
	for (Span *span = begin; span != spanList.end(); span = span->_next)
	{
		if (!span->_freeList.empty())
		{ // 当前span是有效的
			return span;
		}
	}

	// 2.找不到有效span(spanList为空 or spanList中的span obj都不足)，就要去下层 page cache 申请一个新的span，供上层使用

	// 新的span给多少页合适？
	size_t npage = SizeClass::numFetchPage(bytes);
	Span *span = PageCache::getInstance()->newSpan(npage);

	size_t size = SizeClass::roundUp(bytes);
	// 对span分配到的大块内存进行切分，每一块的大小为size
	char *start = (char *)(span->_pageID << PAGE_SHIFT);
	char *end = (char *)((span->_pageID + span->_npage) << PAGE_SHIFT);
	char *cur = start;
	while (cur + 2 * size < end)
	{
		FreeList::nextObj(cur) = cur + size;
		cur = (char *)FreeList::nextObj(cur);
	}
	FreeList::nextObj(cur) = nullptr;
	span->_freeList = FreeList(start);

	// 此时span的自由链表中已经包含充足的内存对象

	assert(span);
	return span;
}

size_t cc_memory_pool::CentralCache::getRangeObj(void *&begin, void *&end, size_t fetchNum, size_t bytes)
{
	// 由bytes映射桶，找到对应的span_list
	size_t idx = SizeClass::index(bytes);

	// 从span_list获取fetchNum个内存对象
	// std::unique_lock<std::mutex> lockguard(_spanLists[idx].getMutex());

	// 获取一个有效的Span
	Span *effectiveSpan = getOneEffectiveSpan(_spanLists[idx], bytes);
	assert(effectiveSpan);

	size_t actualNum = effectiveSpan->_freeList.popNum(begin, end, fetchNum);
	return actualNum;
}
