#include "CentralCache.hh"

CentralCache* CentralCache::getInstance() { return &_instance; }

CentralCache CentralCache::_instance;

// 找到一个非空的Span
Span* CentralCache::getOneSpan(size_t idx) {
    // TODO
    return nullptr;
}

size_t CentralCache::getRangeObj(void* begin, void* end, size_t fetch_num, size_t bytes) {
    // 由bytes映射桶，找到对应的span_list
    size_t idx = SizeClass::index(bytes);

    // 从span_list获取fetchNum个内存对象
    std::unique_lock<std::mutex> lockguard(_span_lists[idx].getMutex());

    Span* goodSpan = getOneSpan(idx);

    size_t actualNum = goodSpan->_free_list.popNum(begin, end, fetch_num);
    return actualNum;
}
