#include "common.hh"

cc_memory_pool::FreeList::FreeList(void* head)
	: _freeList(head), _maxFetchNum(1)
{
}

void*& cc_memory_pool::FreeList::head() {
	return _freeList;
}

void cc_memory_pool::FreeList::push(void* obj)
{ // 头插
	assert(obj != nullptr);
	nextObj(obj) = _freeList;
	_freeList = obj;
}
void cc_memory_pool::FreeList::pushRange(void* begin, void* end)
{
	assert(begin && end);
	nextObj(end) = _freeList;
	_freeList = begin;
}

void* cc_memory_pool::FreeList::pop()
{
	if (_freeList == nullptr)
	{
		return nullptr;
	}
	void* ret = _freeList;
	_freeList = nextObj(_freeList);
	return ret;
}

size_t cc_memory_pool::FreeList::popNum(void*& begin, void*& end, size_t fetchNum)
{ 
	// pop n个节点，如果不够，就全部pop出去
	void* cur = head();
	int cnt = 1;
	while (nextObj(cur) != nullptr && cnt < fetchNum)
	{
		cur = nextObj(cur);
		cnt++;
	}

	begin = head();
	head() = nextObj(cur);

	nextObj(cur) = nullptr;
	end = cur;

	return cnt;
}

// 能从中央缓存获取的最大obj数量
size_t& cc_memory_pool::FreeList::maxFetchNum()
{
	return _maxFetchNum;
}

bool cc_memory_pool::FreeList::empty()
{
	return _freeList == nullptr;
}

void*& cc_memory_pool::FreeList::nextObj(void* obj)
{ // 获取内存对象的下一个对象的地址
	return *(void**)obj;
}