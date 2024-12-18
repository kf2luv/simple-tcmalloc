#include "CentralCache.hh"

cc_memory_pool::CentralCache cc_memory_pool::CentralCache::_instance;

cc_memory_pool::CentralCache* cc_memory_pool::CentralCache::getInstance()
{
	return &_instance;
}

// 获取一个有效的span
cc_memory_pool::Span* cc_memory_pool::CentralCache::getOneEffectiveSpan(SpanList& spanList, size_t bytes)
{

	//(lock) 对中央缓存的某个spanList进行操作，先加桶锁
	{
		std::unique_lock<std::mutex> bucketLock(spanList.getMutex());

		// 1.在Central Cache中映射的spanList中，查找是否存在有效Span
		Span* begin = spanList.begin();
		for (Span* span = begin; span != spanList.end(); span = span->_next)
		{
			if (!span->_freeList.empty())
			{ // 当前span是有效的
				return span;
			}
		}
	}
	//(lock) spanList中无有效span，此时要去下层申请，在这之前先将桶锁释放，让其它线程向该spanList“释放内存”

	// 2.找不到有效span(spanList为空 or spanList中的span obj都不足)，就要去下层 Page Cache 申请一个新的span，供上层使用
	size_t npage = SizeClass::numFetchPage(bytes);
	Span* newSpan = nullptr;

	//(lock) 对页缓存进行操作，加整体锁
	{
		std::unique_lock<std::mutex> pageCacheLock(PageCache::getInstance()->getMutex());
		newSpan = PageCache::getInstance()->newSpan(npage);
		newSpan->_isUsing = true; //标记该span为正在使用，Page Cache不能将其合并
	}

	//(lock) 切割空间的逻辑不用加锁，因为newSpan是当前线程的私有对象

	// 对span分配到的大块内存进行切分，每一块的大小为size
	size_t size = SizeClass::roundUp(bytes);
	char* begin = (char*)(newSpan->_pageID << PAGE_SHIFT);
	char* end = (char*)(begin + (newSpan->_npage << PAGE_SHIFT));
	char* cur = begin;
	while (cur + 2 * size <= end)
	{
		FreeList::nextObj(cur) = cur + size;
		cur = (char*)FreeList::nextObj(cur);
	}
	FreeList::nextObj(cur) = nullptr;
	// 将切分后的自由链表挂载到newSpan上!!!
	newSpan->_freeList = FreeList(begin);

	// 此时newSpan的自由链表中已经包含充足的内存对象，将其插入spanList桶中

	//(lock) 对中央缓存的某个spanList进行操作，先加桶锁
	{
		std::unique_lock<std::mutex> bucketLock(spanList.getMutex());
		spanList.pushFront(newSpan);
	}

	assert(newSpan);
	return newSpan;
}

// begin和end：输出型参数，获取的内存对象（链表形式）的头结点为begin，尾节点为end
// fetchNum：本次从中央缓存获取的内存对象个数
// bytes：获取的内存对象大小
size_t cc_memory_pool::CentralCache::getRangeObj(void*& begin, void*& end, size_t fetchNum, size_t bytes)
{
	// 由bytes映射桶，找到对应的span_list
	size_t idx = SizeClass::index(bytes);

	// 从span_list获取一个有效的Span
	Span* effectiveSpan = getOneEffectiveSpan(_spanLists[idx], bytes);
	assert(effectiveSpan);

	// 从effectiveSpan中获取fetchNum个对象
	size_t actualNum = effectiveSpan->_freeList.popRange(begin, end, fetchNum);

	effectiveSpan->_useCount += actualNum;

	return actualNum;
}


void cc_memory_pool::CentralCache::releaseObjToCentralCache(FreeList& freeList, size_t bytes)
{
	//1.从free链表中取走一部分内存对象
	size_t idx = SizeClass::index(bytes);
	void* begin = nullptr;
	void* end = nullptr;
	size_t actualNum = freeList.popRange(begin, end, freeList.maxFetchNum());

	//2.遍历每一个内存对象obj，依次归还给Central Cache
	void* obj = begin;
	
	//要对CentralCache的桶操作，加桶锁
	std::unique_lock<std::mutex> bucketLock(_spanLists[idx].getMutex());

	while (obj) {
		void* nextObj = FreeList::nextObj(obj);

		// 1.找到内存对象obj对应的span
		Span* span = PageCache::getInstance()->mapObjToSpan(obj);

		// 2.将obj归还给span
		span->_freeList.push(obj);
		span->_useCount--;

		if (span->_useCount == 0) {
			//合并, 从Central Cache中移除
			span->_freeList = nullptr;
			span->_isUsing = false;
			_spanLists[idx].erase(span);

			bucketLock.unlock();
			//归还给下一层（在此之前先解开桶锁，让其它线程可以访问这个桶）
			{
				//对页缓存进行操作，加整体锁
				std::unique_lock<std::mutex> pageCacheLock(PageCache::getInstance()->getMutex());
				PageCache::getInstance()->releaseSpanToPageCache(span);
			}
			bucketLock.lock();
		}

		// 3.查找下一个内存对象obj
		obj = nextObj;
	}
}